#include <iostream>
#include <fstream>

// OBJ_Loader - .obj Loader
#include "../3rdparty/OBJ-Loader/OBJ_Loader.h"
#include "../include/objLoad.h"

using namespace std;

// Main function
int main(int argc, char* argv[])
{

//*********************************************************************************
/*	PARSER section */
    string descriptionString = \
    "objLoad - testing module part of meshToolbox, which loads and dumps an OBJ mesh file.\
    Based on 3rd party OBJ-Load library (see https://github.com/Bly7).";

    argParser.Description(descriptionString);
    argParser.Epilog("Author: J. Cappelletto (cappelletto [at] gmail [dot] com)\nVisit [https://github.com/cappelletto] for more information\n");
    argParser.Prog(argv[0]);
    argParser.helpParams.width = 120;

    try{
        argParser.ParseCLI(argc, argv);
    }

    catch (args::Help){    // if argument asking for help, show this message
        cout << argParser;
        return 1;
    }
    catch (args::ParseError e){  //if some error ocurr while parsing, show summary
        cerr << e.what() << endl;
        cerr << "Use -h, --help command to see usage" << endl;
        return 1;
    }
    catch (args::ValidationError e){ // if some error at argument validation, show
        cerr << "Bad input commands" << endl;
        cerr << "Use -h, --help command to see usage" << endl;
        return 1;
    }
	/*********************************************************************************
     * Start parsing mandatory arguments
     */

    if (!argInput){
        cerr << "Mandatory <input> file name missing" << endl;
        cerr << "Use -h, --help command to see usage" << endl;
        return 1;
    }

    if (!argOutput){
        cerr << "Mandatory <output> file name missing" << endl;
        cerr << "Use -h, --help command to see usage" << endl;
        return 1;
    }

    string InputFile = args::get(argInput);		//String containing the input file path+name from cvParser function
    string OutputFile = args::get(argOutput);	//String containing the output file template from cvParser function

    cout << "***************************************" << endl;
    cout << "Input: " << InputFile << endl;
    cout << "Output: " << OutputFile << endl;

	//-------------------------------------------------------------------------------------------------------------	
	// Initialize Loader object
	objl::Loader Loader;
	bool loadout = Loader.LoadFile(InputFile);

	// Check to see if it loaded correctly
	if (!loadout)
	{
		cout << "Unable to open: " << InputFile << endl;
		return -1;
	}
	// If so continue
	else
	{
		// Create/Open OutputFile
		std::ofstream file(OutputFile);
		cout << "Number of meshes: " << Loader.LoadedMeshes.size() << endl; 

		// Go through each loaded mesh and out its contents
		for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
		{
			// Copy each one of the loaded meshes to be our current mesh
			objl::Mesh curMesh = Loader.LoadedMeshes[i];
			// Print Mesh Name
			cout << "\tCurrent mesh: " << curMesh.MeshName << endl;
			// Print Vertices
			cout << "\t\tVertices: " << curMesh.Vertices.size() << endl;

			int nFaces = curMesh.Indices.size()/3;
			// Print Indices (number of faces)
			cout << "\t\tFaces: " << nFaces << endl;

			// Create a vector that will contain the area of each face (triangle)
			vector <float> faceArea(nFaces);
			vector <objl::Vector3> faceNormal(nFaces);
			vector <float> faceAngle(nFaces);			

			// Go through every 3rd index and print the
			//	triangle that these indices represent
			for (int j = 0; j < nFaces; j++)
			{
				// faces are defined counter-clockwise
				objl::Vector3 A, B, C, U, V;
				// A B C are the vertex that define each triangle

				A = curMesh.Vertices[curMesh.Indices[3*j]].Position;
				B = curMesh.Vertices[curMesh.Indices[3*j+1]].Position;
				C = curMesh.Vertices[curMesh.Indices[3*j+2]].Position;

				// in order to obtain the triangle (face) area, we must find the cross product of the vectors defining the triangle
				U = B - A;
				V = C - A;

				// the cross product gives us the vector normal to the triangle (positive oriented)
				objl::Vector3 R = objl::math::CrossV3(U,V);
				faceNormal[j] = R;

				// the magnitud of that vector is twice the area of the triangle
				faceArea[j] = objl::math::MagnitudeV3(R)/2;
				// totalArea += faceArea[j];

				// finally, for the angle we have: U.V = |U||V|.cos(angle)
				objl::Vector3 UP(0,0,1);	// we create a base vector in Z+
				faceAngle[j] = acos (objl::math::DotV3(UP,R) / (objl::math::MagnitudeV3(UP)*objl::math::MagnitudeV3(R)));
				file << faceArea[j] << "\t" << faceAngle[j] << endl;
			}
			// Leave a space to separate from the next mesh
			file << endl;
		}

		file.close();		// Close File
	}

	return 0;	// exit without a hitch
}
