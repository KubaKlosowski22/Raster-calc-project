#include <stdio.h>
#include <stdlib.h>
#include <gdal/gdal.h>
#include <gdal/cpl_conv.h>

int main(){
GDALDatasetH lay1_dataset, lay2_dataset, dst_dataset;
GDALRasterBandH lay1_band, lay2_band, dst_band;
GDALDriverH driver;
double   geo_transform[6]={0};
double *layer1_buffer, *layer2_buffer;
double *dst_buffer;
char *lay1, *lay2, *dst_file;
const char *wkt;
int  row, choice;

GDALAllRegister();

lay1 = (char*)malloc(101);
lay2 = (char*)malloc(101);
dst_file = (char*)malloc(101);

printf("Podaj nazwę pliku z pierwszą warstwą: ");
scanf("%100s", lay1);
lay1_dataset = GDALOpen(lay1, GA_ReadOnly);

if (lay1_dataset == NULL){
    printf("Nie mogę otworzyć pliku z pierwszą warstwą");
    exit(0);
}

printf("Podaj nazwę pliku z drugą warstwą: ");
scanf("%100s", lay2);
lay2_dataset = GDALOpen(lay2, GA_ReadOnly);

if (lay2_dataset == NULL){
    printf("Nie mogę otworzyć pliku z drugą warstwą");
    exit(0);
}

driver = GDALGetDriverByName("GTiff");
if (driver == NULL){
    printf("Problem z driverem Geotiff!");
}
lay1_band = GDALGetRasterBand(lay1_dataset, 1);
lay2_band = GDALGetRasterBand(lay2_dataset, 1);

int cols1 = GDALGetRasterBandXSize(lay1_band);
int cols2 = GDALGetRasterBandXSize(lay2_band);
int rows1 = GDALGetRasterBandYSize(lay1_band);
int rows2 = GDALGetRasterBandYSize(lay2_band);

printf("Podaj nazwę pliku wynikowego: ");
scanf("%100s", dst_file);

dst_dataset = GDALCreate(driver, dst_file, cols1, rows1,1,GDT_Float64,NULL);
if(!dst_dataset){
    printf("Problem z utworzeniem plik!\n");
    exit(0);
} 
wkt = GDALGetProjectionRef(lay1_dataset);
GDALGetGeoTransform(lay1_dataset, geo_transform);

layer1_buffer= (double *)CPLMalloc(cols1*sizeof(double));
layer2_buffer= (double *)CPLMalloc(cols2*sizeof(double));
dst_buffer = (double *)CPLMalloc(cols1*sizeof(double));

dst_band = GDALGetRasterBand(dst_dataset, 1);
GDALSetProjection(dst_dataset, wkt);
GDALSetGeoTransform(dst_dataset, geo_transform);

printf("Wybierz działanie:\n");
printf("1 - dodawanie\n");
printf("2 - odejmowanie\n");
printf("3 - mnożenie\n");
printf("4 - dzielenie\n");
scanf("%d", &choice);

for (row = 0  ; row <rows1  ; row++) {
    CPLErr err;
    err = GDALRasterIO(lay1_band, GF_Read, 0, row, cols1, 1, layer1_buffer, cols1, 1, GDT_Float64, 0 ,0);	
    if (err>CE_Warning){
	printf("Błąd odczytu z pliku 1!\n");
	break;
    }
    else{
	err = GDALRasterIO(lay2_band, GF_Read, 0, row, cols1, 1,layer2_buffer, cols1, 1, GDT_Float64, 0, 0);
	if(err>CE_Warning){
	    printf("Błąd odczytu z pliku!\n");
	    break;
	}
    }
if (choice == 1){    
    for (int col = 0; col < cols1; col++){
        dst_buffer[col] = layer1_buffer[col] + layer2_buffer[col];
}
    
}
if (choice == 2){
    for (int col = 0; col < cols1; col++){
	dst_buffer[col] = layer1_buffer[col] - layer2_buffer[col];
    }

}

if (choice == 3){
    for (int col = 0; col < cols1; col++){
	dst_buffer[col] = layer1_buffer[col] * layer2_buffer[col];
    }
    
}
if (choice == 4){
    for (int col = 0; col < cols1; col++){
	dst_buffer[col] = layer1_buffer[col] / layer2_buffer[col];
    }
    
}

err = GDALRasterIO(dst_band, GF_Write, 0,row,cols1,1,dst_buffer,cols1,1,GDT_Float64,0,0);
if(err>CE_Warning){
    printf("Błąd zapisu do pliku!\n");
    break;
}
}
GDALClose(lay1_dataset);
GDALClose(lay2_dataset);
GDALClose(dst_dataset);
CPLFree(layer1_buffer);
CPLFree(layer2_buffer);
CPLFree(dst_buffer);
free(lay1);
free(lay2);
free(dst_file);

}