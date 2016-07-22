#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <stdlib.h>
#include <vector>
#include <chrono>

#define __CL_ENABLE_EXCEPTIONS

#include <OpenCL/cl.hpp>

int main( int argc, char** argv ) {
    
    const int N_ELEMENTS= 67108864;

    unsigned int platform_id=0;
    unsigned int device_id;
    if (argc == 2){
        device_id = atoi(argv[1]);
    }else{
        device_id = 0;
    }
    try{
        std::unique_ptr<int[]> A(new int[N_ELEMENTS]); // Or you can use simple dynamic arrays like: int* A = new int[N_ELEMENTS];
        std::unique_ptr<int[]> B(new int[N_ELEMENTS]);
        std::unique_ptr<int[]> C(new int[N_ELEMENTS]);
        
        for( int i = 0; i < N_ELEMENTS; ++i ) {
            A[i] = i;
            B[i] = i;
        }

        // Query for platforms
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        // MCS
        // Get platform info
        std::cout << "Platform Info\n=============\n\n";
        for (auto i = platforms.begin(); i != platforms.end(); i++)
        {
            std::string profile;
            std::string name;
            std::string version;
            std::string vendor;
            std::string extensions;
            i -> getInfo( CL_PLATFORM_PROFILE, &profile);
            i -> getInfo( CL_PLATFORM_NAME, &name);
            i -> getInfo( CL_PLATFORM_VERSION, &version);
            i -> getInfo( CL_PLATFORM_VENDOR, &vendor);
            i -> getInfo( CL_PLATFORM_EXTENSIONS, &extensions);
            std::cout << "Profile:    " << profile << std::endl;
            std::cout << "Name:       " << name << std::endl;
            std::cout << "Version:    " << version << std::endl;
            std::cout << "Vendor:     " << vendor << std::endl;
            std::cout << "Extensions: " << extensions << std::endl;
            std::cout << "\n\n";
        }

        // Get a list of devices on this platform
        std::vector<cl::Device> devices;
        platforms[platform_id].getDevices(CL_DEVICE_TYPE_GPU|CL_DEVICE_TYPE_CPU, &devices); // Select the platform.


        std::cout << "Device Info (selected device in parenthesis)\n===========\n\n";
        unsigned int loopIdx = 0;
        size_t maxWorkGroup;
        for (auto i = devices.begin(); i != devices.end(); i++)
        {
            std::string name;
            cl_device_type type;
            std::string profile;
            std::string version;

            i -> getInfo( CL_DEVICE_NAME, &name);
            i -> getInfo( CL_DEVICE_TYPE, &type);
            i -> getInfo( CL_DEVICE_MAX_WORK_GROUP_SIZE, &maxWorkGroup);
            i -> getInfo( CL_DEVICE_PROFILE, &profile);
            i -> getInfo( CL_DEVICE_VERSION, &version);

            if( loopIdx == device_id ){
                std::cout << "Name:         " << "(((" << name << ")))" << std::endl;
            }else{
                std::cout << "Name:         " << name << std::endl;
            }
            std::cout << "Type:         " << type << std::endl;
            std::cout << "Max Wk Grp:   " << maxWorkGroup << std::endl;
            std::cout << "Profile:      " << profile << std::endl;
            std::cout << "Version:      " << version << std::endl;
            std::cout << "\n\n";
    
            loopIdx +=1;
        }

        // Create a context
        cl::Context context(devices);

        // Create a command queue
        cl::CommandQueue queue = cl::CommandQueue( context, devices[device_id] );   // Select the device.

        // Create the memory buffers
        cl::Buffer bufferA=cl::Buffer(context, CL_MEM_READ_ONLY, N_ELEMENTS * sizeof(int));
        cl::Buffer bufferB=cl::Buffer(context, CL_MEM_READ_ONLY, N_ELEMENTS * sizeof(int));
        cl::Buffer bufferC=cl::Buffer(context, CL_MEM_WRITE_ONLY, N_ELEMENTS * sizeof(int));

        // Copy the input data to the input buffers using the command queue.
        queue.enqueueWriteBuffer( bufferA, CL_FALSE, 0, N_ELEMENTS * sizeof(int), A.get() );
        queue.enqueueWriteBuffer( bufferB, CL_FALSE, 0, N_ELEMENTS * sizeof(int), B.get() );

        // Read the program source
        std::ifstream sourceFile("vector_add_kernel.cl");
        std::string sourceCode( std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));
        cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()));

        // Make program from the source code
        cl::Program program=cl::Program(context, source);

        // Build the program for the devices
        program.build(devices);

        // Make kernel
        cl::Kernel vecadd_kernel(program, "vecadd");

        // Set the kernel arguments
        vecadd_kernel.setArg( 0, bufferA );
        vecadd_kernel.setArg( 1, bufferB );
        vecadd_kernel.setArg( 2, bufferC );


    auto t1 = std::chrono::high_resolution_clock::now();



        // Execute the kernel
        cl::NDRange global( N_ELEMENTS );

       
          
        cl::NDRange local( maxWorkGroup ); // invalid work group size? depends on device
        queue.enqueueNDRangeKernel( vecadd_kernel, cl::NullRange, global, local );

        // Copy the output data back to the host
        queue.enqueueReadBuffer( bufferC, CL_TRUE, 0, N_ELEMENTS * sizeof(int), C.get() );


    auto t2 = std::chrono::high_resolution_clock::now();

    std::cout << "Kernel executed in " << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
                << " milliseconds" << std::endl;


        // Verify the result
        bool result=true;
        for (int i=0; i<N_ELEMENTS; i ++)
            if (C[i] !=A[i]+B[i]) {
                result=false;
                break;
            }
        if (result)
            std::cout<< "Success!\n";
        else
            std::cout<< "Failed!\n";

    }
    catch(cl::Error err) {
        std::cout << "Error: " << err.what() << "(" << err.err() << ")" << std::endl;
        return( EXIT_FAILURE );
    }

    std::cout << "Done.\n";
    return( EXIT_SUCCESS );
}
