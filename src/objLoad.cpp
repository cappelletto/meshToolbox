#include <iostream>
#include <fstream>

// OBJ_Loader - .obj Loader
#include "../3rdparty/OBJ-Loader/OBJ_Loader.h"
#include "../include/options.h"

using namespace std;

// Main function
int main(int argc, char* argv[])
{

//*********************************************************************************
/*	PARSER section */
    string descriptionString = \
    "objLoad - testing module which loads and dump an OBJ mesh file.\
    Based on 3rd party OBJ-Load library (see README.md).";

    argParser.Description(descriptionString);
    argParser.Epilog("Author: J. Cappelletto (GitHub: @cappelletto)\nVisit [https://github.com/cappelletto] for more information\n");
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

    string InputFile = args::get(argInput);	//String containing the input file path+name from cvParser function
    string OutputFile = args::get(argOutput);	//String containing the output file template from cvParser function

    cout << "***************************************" << endl;
    cout << "Input: " << InputFile << endl;
    cout << "Output: " << OutputFile << endl;

	//-------------------------------------------------------------------------------------------------------------	
	// Initialize Loader object
	objl::Loader Loader;

	// Load .OBJ File
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

		// Go through each loaded mesh and out its contents
		for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
		{
			// Copy one of the loaded meshes to be our current mesh
			objl::Mesh curMesh = Loader.LoadedMeshes[i];

			// Print Mesh Name
			file << "Mesh " << i << ": " << curMesh.MeshName << "\n";

			// Print Vertices
			file << "Vertices:\n";

			// Go through each vertex and print its number,
			//  position (P[3]), normal (N[3]), and texture coordinate (TC[2])
			for (int j = 0; j < curMesh.Vertices.size(); j++)
			{
				file << "V" << j << ": " <<
					"P(" << curMesh.Vertices[j].Position.X << ", " << curMesh.Vertices[j].Position.Y << ", " << curMesh.Vertices[j].Position.Z << ") " <<
					"N(" << curMesh.Vertices[j].Normal.X << ", " << curMesh.Vertices[j].Normal.Y << ", " << curMesh.Vertices[j].Normal.Z << ") " <<
					"TC(" << curMesh.Vertices[j].TextureCoordinate.X << ", " << curMesh.Vertices[j].TextureCoordinate.Y << ")\n";
			}

			// Print Indices
			file << "Indices:\n";

			// Go through every 3rd index and print the
			//	triangle that these indices represent
			for (int j = 0; j < curMesh.Indices.size(); j += 3)
			{
				file << "T" << j / 3 << ": " << curMesh.Indices[j] << ", " << curMesh.Indices[j + 1] << ", " << curMesh.Indices[j + 2] << "\n";
			}

			// Print Material
			file << "Material: " << curMesh.MeshMaterial.name << "\n";
			file << "Ambient Color: " << curMesh.MeshMaterial.Ka.X << ", " << curMesh.MeshMaterial.Ka.Y << ", " << curMesh.MeshMaterial.Ka.Z << "\n";
			file << "Diffuse Color: " << curMesh.MeshMaterial.Kd.X << ", " << curMesh.MeshMaterial.Kd.Y << ", " << curMesh.MeshMaterial.Kd.Z << "\n";
			file << "Specular Color: " << curMesh.MeshMaterial.Ks.X << ", " << curMesh.MeshMaterial.Ks.Y << ", " << curMesh.MeshMaterial.Ks.Z << "\n";
			file << "Specular Exponent: " << curMesh.MeshMaterial.Ns << "\n";
			file << "Optical Density: " << curMesh.MeshMaterial.Ni << "\n";
			file << "Dissolve: " << curMesh.MeshMaterial.d << "\n";
			file << "Illumination: " << curMesh.MeshMaterial.illum << "\n";
			file << "Ambient Texture Map: " << curMesh.MeshMaterial.map_Ka << "\n";
			file << "Diffuse Texture Map: " << curMesh.MeshMaterial.map_Kd << "\n";
			file << "Specular Texture Map: " << curMesh.MeshMaterial.map_Ks << "\n";
			file << "Alpha Texture Map: " << curMesh.MeshMaterial.map_d << "\n";
			file << "Bump Map: " << curMesh.MeshMaterial.map_bump << "\n";

			// Leave a space to separate from the next mesh
			file << "\n";
		}

		// Close File
		file.close();
	}

	// Exit the program
	return 0;
}
