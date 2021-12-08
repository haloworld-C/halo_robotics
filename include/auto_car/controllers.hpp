/*
Longtitude controller 
modified by halo
20211125
*/
# ifndef _CONTROLLERS_
# define _CONTROLLERS_
# include <cmath>

//PID controller
template<typename dataType>
class PIDcontroller{
    private:
	enum mode{stop, run, start};//record car status
    dataType ki_; //PID parameter of integration
    dataType kd_; //PID parameter of derivative
    dataType kp_; //PID parameter of gain
	dataType ki_big_; //pid_i for initial start and stop
    dataType T_;  //control preriod
    dataType lastErr_; // last step err
    dataType errSum_; // record last step error
    dataType integrationLimit_;
	dataType integrationLimitHigh_;
    dataType thottleLimit_;
	dataType outLimit_initialAndStop_;
	dataType outLimit_run_;
    dataType brakeLimit_;
    dataType lastOutput_;
    dataType deadZone_;
    dataType targetSpeed_;
	dataType offset_;
	dataType forwardFeed_;
    int integrationSaturationFlag_;//-1：积分负饱和;0,积分未饱和;1：积分正饱和
    bool outputLimitFlag_;
    //bool firstHit_; //记录车辆是否为从速度为零开始启动状态
	mode runFlag_;
    public:
	PIDcontroller()=default;
    PIDcontroller(dataType kp , dataType ki, dataType kd, dataType T = 0.01, dataType integrationLimit = 300, dataType deadZone = 0.1):
         T_(T), integrationLimit_(integrationLimit), deadZone_(deadZone) {
	 ki_ = ki;
	 kp_ = kp;
	 kd_ = kd;
	 ki_big_ = ki_ * 100;
	 if(ki_big_ > 0.5){ki_big_ = 0.5;}
	 outputLimitFlag_ = true;
	 runFlag_ = mode::start;
}//后续把定义与声明分开
	void setPIDParameters(dataType kp, dataType ki, dataType kd);
    dataType PIDCaculator(dataType vRef, dataType vCurrent);
	dataType basicPIDCaculator(dataType vRef, dataType vCurrent);
    dataType getLastOutput()const{return lastOutput_;}
    void setOutputLimit(dataType, dataType);
    void setDeadZone(dataType deadZone){deadZone_ = deadZone;}
    void setTargetSpeed(dataType targetSpeed){targetSpeed_ = targetSpeed;}
    dataType getTargetSpeed()const{return targetSpeed_;}
	std::string getRunMode()const{return std::to_string(runFlag_);}
	//TODO
	//可能需要启用计时器timer
};
template<typename dataType>
inline void 
PIDcontroller<dataType>::setPIDParameters(dataType kp, dataType ki, dataType kd){
    ki_ = ki;
	ki_big_ = ki * 100;
	if(ki_big_ > 0.5){ki_big_ = 0.5;}//maximum of ki_big_ is 0.5
    kd_ = kd;
    kp_ = kp;
}

template<typename dataType>
inline dataType
PIDcontroller<dataType>::PIDCaculator(dataType vRef, dataType vCurrent){
    dataType outputCurrent;
	if(vRef < 0.2){runFlag_ = mode::stop;}//当指令速度非常小时，认为是刹停命令
    dataType err = vRef - vCurrent;
    if(abs(err) < deadZone_){err = 0;}//设置死区
    if(vCurrent > 2 && err < 2){runFlag_ = mode::run;}
	if(runFlag_ == mode::run){
		if(err > 2){err = 2;}
		if(err < -2){err = -2;}//设置误差上限制，防止系统响应过快
		                       //仅当汽车运行于平稳状态时生效，在起步与刹停阶段应使相应比较快
		if((lastErr_ > -0.2 && err < -0.2) || (lastErr_ < 0.2 && err > 0.2)){
			errSum_ = 0;}//处理积分穿越问题，防止刹车与油门互相切换导致的指令突变
						 //TODO：REVIEW
	}
	errSum_ += err * T_;

    if(errSum_ < -integrationLimit_){
        integrationSaturationFlag_ = -1;
        errSum_ = -integrationLimit_;
    }
    else if(errSum_ > integrationLimit_){
        integrationSaturationFlag_ = 1;
        errSum_ = integrationLimit_;
    }
    else{
        integrationSaturationFlag_ = 0;
    }//设置基本饱和
	//当启动与刹停时，将ki系数设置到一个较大值
    double ki_current = (runFlag_ == mode::run) ? ki_ : ki_big_;   
	outputCurrent = kp_ * err + ki_current * errSum_ + kd_ * (err - lastErr_) / T_;//除非设定，kd默认为0,微分项不起作用
    //限制控制器输出，防止损伤作动器
	if(outputLimitFlag_ == true){
		if(outputCurrent < 0) outputCurrent /= 4;//油门的指令比刹车扩大4倍
        if(outputCurrent > thottleLimit_){outputCurrent = thottleLimit_;}
    	if(outputCurrent < brakeLimit_){outputCurrent = brakeLimit_;}
	}
    lastErr_ = err;  
    lastOutput_ = outputCurrent;
    
    return outputCurrent;
}
template<typename dataType>
inline dataType
PIDcontroller<dataType>::basicPIDCaculator(dataType vRef, dataType vCurrent){
	dataType err = vRef - vCurrent;
	errSum_ += err * T_;
	dataType currentOutput = kp_ * err + ki_ * errSum_ + kd_ * (err - lastErr_) / T_;
	lastErr_ = err;
	lastOutput_ = currentOutput;

	return currentOutput;
}

template<typename dataType>
inline void
PIDcontroller<dataType>::setOutputLimit(dataType thottleLimit, dataType brakeLimit){
    thottleLimit_ = thottleLimit;
    brakeLimit_ = brakeLimit;
    outputLimitFlag_ = true;
} 
# endif
