# include <iostream>
# include <Eigen/Dense>
# include <vector>
# include <string>
# include <cmath>
# include <libconfig.h++>//libconfig library for store and parse data
# include <unistd.h>
# define NDEBUG//weather to open assert detection
using namespace std;
using namespace libconfig;
//test for interplation
namespace{
	string cwd = string(get_current_dir_name());
	string config_path = cwd + "/config/example.conf";
}//namespace
void generateAxis(vector<double>& vectorAxis, double upper, double lower, double step, string type){
	//to do: check whether upper is greater than lower value
	int scale = (upper - lower) / step;
	if(type == "x"){
		for(int counter = 0; counter < scale ; counter++){
			for(int repeatCounter = 0; repeatCounter < scale; repeatCounter++){
				vectorAxis.push_back(lower + counter * step);
			}
		}
    }
	if(type == "y"){
		for(int counter = 0; counter < scale ; counter++){
			for(int repeatCounter = 0; repeatCounter < scale; repeatCounter++){
				vectorAxis.push_back(lower + repeatCounter * step);
			}
		}
	}
}
void generateZ(vector<double>& x, vector<double>& y, vector<double>& z){
	//to do: other generate type
	auto iter = y.begin();
	for(auto& xi : x){
		z.push_back(sqrt(xi * xi + (*iter) * (*iter)));
		iter++;
	}
}
void print_axis(vector<double> ob){
	for(auto& i : ob){
		std::cout << i << "\t";
	}
	std::cout << std::endl;
}

void configTestWrite(){
	Config cfg;
	Setting &root = cfg.getRoot();
	//Add some setting to the configuration
	Setting &address = root.add("address", Setting::TypeGroup);
	address.add("x", Setting::TypeFloat) = 1.0;
	address.add("y", Setting::TypeFloat) = 1.0;
	address.add("z", Setting::TypeFloat) = 1.0;

	try{
		cfg.writeFile(config_path);
		cerr << "New configuration successfully written to: " << config_path << endl;
	}
	catch(const FileIOException &fioex){
		cerr << "I/O error while writing file: " << config_path << endl;
	}
	
}
void configTestRead(){
	Config cfg;
	// Read the file. If there is an error, report it and exit.
	try{
		cfg.readFile(config_path);
	}
	catch(const FileIOException &fioex){
		cerr << "I/O error while reading file." << endl;
	}
	catch(const ParseException &pex){
		std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() << std::endl;
	}
	//const Setting &root = cfg.getRoot();
	//const Setting &data = root["lon_controller_conf"];
	//double xValue;
	//data.lookupValue("x", xValue);
	cout << "successing reading data so far" << endl;
}
int main(){
	cout << "Test for config." << endl;
	//vector<double> x, y, z;
	
	//fill data
	//_1D::CubicSplineInterpolator<double> interp;
	//interp.setData(x, y);//fill data
	//double val = interp(2.5);//interpolation operation
	//cout << "result: " << val << endl;
	//test for generateAxis
	//generateAxis(x, 8, -8, 0.5, "x");
	//generateAxis(y, 8, -8, 0.5, "y");
	//generateZ(x, y, z);
	//cout << "size of x: " << x.size() << endl;
	//assert(x.size() == y.size());
	//_2D::ThinPlateSplineInterpolator<double> interp2D;
	//interp2D.setData(x, y, z);
	//double val2D = interp2D(0.3, 0.5);

	//cout << "result of 2D interplation: " << val2D << endl;
	//test for string path
	//cout << "write directroy path: " << config_path << endl; 
    //configTestWrite();
	configTestRead();

	return 0;
}

