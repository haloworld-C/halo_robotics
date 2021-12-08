/*
this is a head-only C++ file.
The class here intend to record and interplolation the controller cmd need
*/
# ifndef _CMDADAPTOR_
# define _CMDADAPTOR_
# include <libInterpolate/Interpolate.hpp>
# include <libconfig.h++>
# include <Eigen/Dense>
# include <string>
# include <iostream>

using namespace libconfig;
using namespace Eigen;
class cmdAdaptorBase{
	public:
	cmdAdaptorBase(std::string filePath);
	bool hasData(){return hasData_;}
	void recordIMU(double a, double cmd, double speed);//when we have IMD, we record a directly
	void recordNIMU(double dt, double cmd, double speed){};//when we have not IMU, we need dt to 
														//caculate value of a
	void readConf();
	void writeConf();
	int idenfication(){return 0;}
	double calibration(double a, double speed)const;
	//virtual double callibrition(double a, double v); //impelement of 2D interpolation
	~cmdAdaptorBase();

	private:
	void setData();
	std::string filePath_;
	double lastSpeed_;
	int weight_;    //somehow if we have a way to know the mass infomation,then we can take advantageof it
	double offset_; //we can adjust the cmd by an offset to accelaration on condition
	Config cfg;
	_2D::ThinPlateSplineInterpolator<double> interp2D;
	bool hasData_; //flag of whether having dataset
	Matrix<double, Dynamic, 3> data_; //store dataset reading from config file
	Matrix<double, Dynamic, 3> recorder_;//dataset recorder from veicle
										//it will write to config file later
	int maxDataSize;
};
inline
cmdAdaptorBase::cmdAdaptorBase(std::string filePath){
	//check whether config file has data
	filePath_ = filePath + "calibration.conf";
	weight_ = -1;// -1 representing we don't know the car's weight
	hasData_ = false;
	maxDataSize = 1000000;//max data to maintain
}
inline void
cmdAdaptorBase::readConf(){
	try{
		cfg.readFile(filePath_);
	}
	catch(const FileIOException &fioex){
		std::cerr << "class construction failed because of 'I/O error while reading file'" 
		          << filePath_ << std::endl;
	}
	catch(const ParseException &pex){
		std::cerr << "class construction failed because of 'Parse error'" << std::endl;
	}
	try{
		Setting &root = cfg.getRoot();
		if(root.exists("calibration_table")){
			Setting &address = root["calibration_table"];
			if(address.getLength() > 1000){
				hasData_ = true;
			}
			else{
				hasData_ = false;
			}
		}
	}
	catch(const SettingNotFoundException &nfex){
		hasData_ = false;
	}
	if(hasData_){
		setData();
	}
}
inline void
cmdAdaptorBase::writeConf(){
	try{
		cfg.writeFile(filePath_);
		std::cerr << "successed in writing config" << filePath_ << std::endl;
		//hasData_ = true;
	}
	catch(const FileIOException &fioex){
		std::cerr << "File I/O error while writing file of " << filePath_ << std::endl;
	}
}
inline void
cmdAdaptorBase::recordIMU(double a, double cmd, double speed){
	if(cmd != 0){
		static int indexThottle = 0;
		static int indexBrake = 0;

		Setting &root = cfg.getRoot();
		//add data in this frame
		if(! root.exists("calibration_table_thottle")){
			root.add("calibration_table_thottle", Setting::TypeGroup);
		}
		if(! root.exists("calibration_table_brake")){
			root.add("calibration_table_brake", Setting::TypeGroup);
		}
		try{
		std::string catagory;
		std::string title;
		if(cmd > 0){
			catagory = "calibration_table_thottle";
			title = "calibration" + std::to_string(indexThottle);
			indexThottle++;
		}
		else{
			catagory = "calibration_table_brake";
			title = "calibration" + std::to_string(indexBrake);
			indexBrake++;
		}
		Setting &address = root[catagory];
		if(! address.exists(title)){
			address.add(title, Setting::TypeGroup);
		}
		Setting &frame = address[title];
		
		frame.add("accelaration", Setting::TypeFloat) = float(a);
		frame.add("cmd", Setting::TypeFloat) = float(cmd);
		frame.add("speed", Setting::TypeFloat) = float(speed);
		}
		catch(const SettingTypeException sets){
			std::cerr << "record data failed" << std::endl;
		}
		catch(const SettingNameException nameErr){
			std::cerr << "Setting Name Error!" << std::endl;
		}
    }
		
}
inline void
cmdAdaptorBase::setData(){
	if(hasData_){
		Setting &root = cfg.getRoot();
		//load data from config to data set
		Setting &cali = root["calibration_table"];
		int dataLength = cali.getLength();
		data_.resize(dataLength ,3);
		int columCounter = 0;
		for(auto caliDataIter = cali.begin(); caliDataIter != cali.end(); ++caliDataIter){
			Setting &onePiece = *caliDataIter; 
			double a = onePiece["accelaration"];
			double cmd = onePiece["cmd"];
			double speed = onePiece["speed"];
			data_.row(columCounter)	<< a, cmd, speed;
			columCounter++;
		}
		interp2D.setData(data_.col(0), data_.col(2), data_.col(1));
		std::cout << "set " << dataLength << " data for interpolation succeed!" << std::endl;
	}
	else
		std::cerr << "set data fail because of lack of data in configFile.." << std::endl;

}
inline double
cmdAdaptorBase::calibration(double a, double speed)const{
	return interp2D(a, speed);
}
inline 
cmdAdaptorBase::~cmdAdaptorBase(){
}
# endif
