#include <stdio.h>
#include <stdint.h>

#include "model.h"
#include "stl.h"


namespace stl
{
	enum State
	{
		State_Start,
		State_Reading_Tris,
	};


#pragma pack(2)
	struct tri
	{
		float normal[3];
		float v1[3];
		float v2[3];
		float v3[3];
		uint16_t flags;
	};

	bool convert(const char *stlFileName, Model *outModel)
	{
		printf("Converting STL data\n");

		bool retval = false;
		FILE *fin = fopen(stlFileName, "rb");
		if (fin)
		{
			State readState = State_Start;
			outModel->filename = stlFileName;
			tri buffer;
			uint8_t header[80];

			outModel->bbMin = Vec3(9999.0f, 9999.0f, 9999.0f);
			outModel->bbMax = Vec3(-9999.0f, -9999.0f, -9999.0f);

			uint32_t triCount;
			uint32_t index = 0;
			while (!feof(fin))
			{
				switch(readState)
				{
					case State_Start:
						fread(header, sizeof(header), 1, fin);
						fread(&triCount, sizeof(triCount), 1, fin);
						printf("Triangle Count = %d\n", triCount);
						readState = State_Reading_Tris;
						// Allocate the sapce for the tris
						outModel->AllocTris(triCount);
						break;
					case State_Reading_Tris:
						printf("reading tri %06d\r", index);
						fread(&buffer, sizeof(buffer), 1, fin);

						outModel->tris[index].normal = buffer.normal;
						outModel->tris[index].v1 = buffer.v1;
						outModel->tris[index].v2 = buffer.v2;
						outModel->tris[index].v3 = buffer.v3;

						Vec3::Min(&outModel->bbMin, outModel->bbMin, outModel->tris[index].v1);
						Vec3::Min(&outModel->bbMin, outModel->bbMin, outModel->tris[index].v2);
						Vec3::Min(&outModel->bbMin, outModel->bbMin, outModel->tris[index].v3);

						Vec3::Max(&outModel->bbMax, outModel->bbMax, outModel->tris[index].v1);
						Vec3::Max(&outModel->bbMax, outModel->bbMax, outModel->tris[index].v2);
						Vec3::Max(&outModel->bbMax, outModel->bbMax, outModel->tris[index].v3);


						if (triCount == ++index)
						{
							retval = true;
							goto cleanup;
						}
						break;					
					default:
						printf("\nError reading file\n");
						break;
				}



			}
		}
cleanup:
		printf("\n\n");
		if (fin)
			fclose(fin);


		return retval;
	}
}

