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
			vector <float> 			faceArea(nFaces);
			vector <objl::Vector3> 	faceNormal(nFaces);
			vector <float> 			faceAngle(nFaces);
			vector <objl::Vector3> 	faceCenter(nFaces);				
			vector <int> 			faceFlag(nFaces);				

			// offset vertex cloud to origin (0,0,0)
			objl::Vector3 global_min, global_max;			
			// we use the first point as the min/max for starting
			global_min = global_max = curMesh.Vertices[0].Position;

		//	***********************************************************
		//	Find min/max of vertex
			// we iterate across all the source vertex (it is faster if done before deduplicating vertex-face list)

			for (int i=1; i < nVertex; i++)
			{
				objl::Vector3 currPoint = curMesh.Vertices[i].Position;

				// update maximum values
				if (currPoint.X > global_max.X)	global_max.X = currPoint.X;
				if (currPoint.Y > global_max.Y)	global_max.Y = currPoint.Y;
				if (currPoint.Z > global_max.Z)	global_max.Z = currPoint.Z;

				// update minimum values
				if (currPoint.X < global_min.X)	global_min.X = currPoint.X;
				if (currPoint.Y < global_min.Y)	global_min.Y = currPoint.Y;
				if (currPoint.Z < global_min.Z)	global_min.Z = currPoint.Z;
			}
			// at this point, the bounding box is defined by global_min and global_max vertex. 

		//	***********************************************************
		// Find bbox
			// the upper limit of the bbox is given as the difference between the max and the min
			// we use the first point as the min/max for starting
			global_max = global_max - global_min;

		//	***********************************************************
		// Create grid X-Y
			// for the grid creation, we use the size of the size of a XY cell and the bbox
			//
			cout << "Cell size: " << cellSize << endl;
			int nx = ceil(global_max.X / cellSize);	
				int ny = ceil(global_max.Y / cellSize);	

			cout << "Bounding box: " << global_max.X << " " << global_max.Y << " " << global_max.Z << endl;   
			cout << "Grid dimensions: " << nx << " x " << ny << endl;

			// now we create the container matrix with the max height (Z) value for each cell. This does behave as a Digital Elevation Map
			std::vector<std::vector<float>> DEM(nx, std::vector<float>(ny,0));

		//	***********************************************************
		//	Offset X-Y
			// Now, we proceed to offset all the vertex according the minimum values of X, Y and Z, so the bbox will start from 0,0,0

			for (int i=0; i < nVertex; i++)
			{
				curMesh.Vertices[i].Position = curMesh.Vertices[i].Position - global_min;				
			}			

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
				int cx,cy;
				// find target cell in the XY grid for the resulting face center
				cx = floor (faceCenter[j].X / cellSize);
				cy = floor (faceCenter[j].Y / cellSize);
				// if higher, update DEM value for given cell
				if (faceCenter[j].Z > DEM[cx][cy]) DEM[cx][cy] = faceCenter[j].Z;				
			}			

			// Finally, after updating the DEM array, we can check each face to see if occlusion occurs

		//	***********************************************************
		// For each face:
		// check if lower than Zmax - dZmax
		// tag as covered or not
			for (int j = 0; j < nFaces; j++)
			{
				int cx,cy;
				// find target cell in the XY grid for the resulting face center
				cx = floor (faceCenter[j].X / cellSize);
				cy = floor (faceCenter[j].Y / cellSize);
				// if higher, update DEM value for given cell
				// Check if it
				if (faceCenter[j].Z < (DEM[cx][cy] - ToleranceZ)) 	faceFlag[j] = 0;	// completely covered
				else if (faceCenter[j].Z < DEM[cx][cy])			faceFlag[j] = 1;	// almost covered
				else											faceFlag[j] = 2;	// top
				file << faceCenter[j].X << "\t" << faceCenter[j].Y << "\t" << faceCenter[j].Z << "\t" << faceFlag[j] << endl;
			}
			// export data

		file.close();		// Close File
		}
	}	

	return 0;	// exit without a hitch
}
