#include <iostream>
#include <fstream>

// OBJ_Loader - .obj Loader
#include "../3rdparty/OBJ-Loader/OBJ_Loader.h"
#include "../include/meshAnalize.h"

using namespace std;

// Main function
int main(int argc, char* argv[])
{

//*********************************************************************************
/*	PARSER section */
    string descriptionString = \
    "meshAnalize - testing module part of meshToolbox, which loads OBJ mesh file, and further analize surface, occlusion, normal, angles, etc .\
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

	float cellSize = 5;			//size of each grid cell. Default value: 5
    if (argCellSize){
		cellSize = args::get(argCellSize);			//size of each grid cell (square)
    }
	else
	{
		cout << "Using default value for cellSize = " << cellSize << endl;
	}

	float ToleranceZ = 1;			//Default value: 1
    if (argToleranceZ){
		ToleranceZ = args::get(argToleranceZ);		//value of height tolerance
    }
	else
	{
		cout << "Using default value for ToleranceZ = " << ToleranceZ << endl;
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
		for (int m = 0; m < Loader.LoadedMeshes.size(); m++)
		{
			// Copy each one of the loaded meshes to be our current mesh
			objl::Mesh curMesh = Loader.LoadedMeshes[m];
			// Print Mesh Name
			cout << "\tCurrent mesh: " << curMesh.MeshName << endl;
			// Print Vertices
			cout << "\t\tVertices: " << curMesh.Vertices.size() << endl;

			int nVertex = curMesh.Indices.size();
			int nFaces = nVertex/3;

			// Print Indices (number of faces)
			cout << "\t\tFaces: " << nFaces << endl;

			// Create a vector that will contain the info for each face (triangle)
			vector <float> 			faceArea(nFaces);	// area of current face
			vector <objl::Vector3> 	faceNormal(nFaces);	// normal of current face
			vector <float> 			faceAngle(nFaces);	// angle against normal of current face
			vector <objl::Vector3> 	faceCenter(nFaces);	// center of current face
			vector <float> 			faceDistance(nFaces);	// minimum distance to plane containing face
			vector <int> 			faceFlag(nFaces, 0);	// flag indicating if face is occluded

		//	***********************************************************
		// For each face:
			for (int j = 0; j < nFaces; j++)
			{
				// faces are defined counter-clockwise
				objl::Vector3 A, B, C, U, V;
				// A B C are the vertex that define each triangle
				// retrieve the corners of each face
				A = curMesh.Vertices[curMesh.Indices[3*j]].Position;
				B = curMesh.Vertices[curMesh.Indices[3*j+1]].Position;
				C = curMesh.Vertices[curMesh.Indices[3*j+2]].Position;

				// in order to obtain the triangle (face) area, we must find the cross product of the vectors defining the triangle
				U = B - A;
				V = C - A;

				//	***********************************************************
				// Compute face normal
				// the cross product gives us the vector normal to the triangle (positive oriented)
				objl::Vector3 R = objl::math::CrossV3(U,V);
				faceNormal[j] = R;
				// find the free parameter D for the plane containing the current face
				faceDistance[j] = objl::math::DotV3(A,R);

				//	***********************************************************
				// Compute face area
				// the magnitud of that vector is twice the area of the triangle
				faceArea[j] = objl::math::MagnitudeV3(R)/2;
				// totalArea += faceArea[j];

				//	***********************************************************
				// Compute face angle
				// finally, for the angle we have: U.V = |U||V|.cos(angle)
				objl::Vector3 UP(0,0,1);	// we create a base vector in Z+
				faceAngle[j] = acos (objl::math::DotV3(UP,R) / (objl::math::MagnitudeV3(UP)*objl::math::MagnitudeV3(R)));

				// find the center of the face (as the average of each corner vertex)
				faceCenter[j] = (A + B + C)/3;

			}			

			// Finally, after updating the DEM array, we can check each face to see if occlusion occurs

		//	***********************************************************
		// For each face:
		// apply raytrace to see current j-face is occluded by any other k-face
		// and tag it properly
	
		cout << "Computing raytrace for all faces" << endl;
			// for raytracing, we see if rect P = O + t.R intersects inside k-face, defined in the plane form Ax + By + Cz + D = 0 
			for (int j = 0; j < (nFaces-1); j++)
			{
				// P is defined as the center of the current j-face
				objl::Vector3 O = faceCenter[j];

				objl::Vector3 R;
				R.X = R.Y = 0; R.Z = 1;	// normal vector pointing from the sky (Z = 0)

				float t;
				objl::Vector3 P;

				for (int k = j+1; k < nFaces; k++)
				{
					// Imperative: discard if line and triangle are parallel, or there won't be any intersection
					if (objl::math::DotV3(R,faceNormal[k]) != 0)
					{
						// For the current k-face employ its plane form. We already have its normal form, and the origin-distance parameter D
						t = -(objl::math::DotV3(faceNormal[k], O) + faceDistance[k]) / (objl::math::DotV3(R, faceNormal[k]));
						P = O + R*t;
						// Given P, we must check if it lies within k-face
						objl::Vector3 V1, V2, V3;
						objl::Vector3 e1, e2, e3;
						objl::Vector3 c1, c2, c3;

						V1 = curMesh.Vertices[curMesh.Indices[3*k]].Position;
						V2 = curMesh.Vertices[curMesh.Indices[3*k+1]].Position;
						V3 = curMesh.Vertices[curMesh.Indices[3*k+2]].Position;

						e1 = V2 - V1;
						e2 = V3 - V2;
						e3 = V1 - V3;

						c1 = P - V1;
						c2 = P - V2;
						c3 = P - V3;

						if ((objl::math::DotV3(faceNormal[k], objl::math::CrossV3(e1,c1)) > 0) &&
						    (objl::math::DotV3(faceNormal[k], objl::math::CrossV3(e2,c2)) > 0) &&
						    (objl::math::DotV3(faceNormal[k], objl::math::CrossV3(e3,c3)) > 0))
							{
								faceFlag[k] = 1;
								break; // we exit from current loop, as this face is already tagged as "covered"
							}
//						else	faceFlag[k] = 0;

					}
				}
				
				file << faceCenter[j].X << "\t" << faceCenter[j].Y << "\t" << faceCenter[j].Z << "\t" << faceFlag[j] << endl;
			}
			// export data

		file.close();		// Close File
		}
	}	

	return 0;	// exit without a hitch
}
