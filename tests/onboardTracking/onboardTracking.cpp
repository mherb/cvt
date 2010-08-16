#include <iostream>


int main(int argc, char* argv[])
{
	std::cout << "Test application for quadrocopter onboard tracking system" << std::endl;
	
	if(argc < 2){
		std::cout << "Usage: " << argv[0] << " <fileName> " << std::endl;
		return 0;
	}
		
		
	std::vector<Eigen::Vector4d> points;
	std::vector<Eigen::Vector4d> edges;
			
	return 0;
}
