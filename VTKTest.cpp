//============================================================================
// Name        : VTKTest.cpp
// Author      : shong
// Version     :
// Copyright   : None
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include <math.h>
using namespace std;

#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"
#include "vtkUnsignedShortArray.h"

#include "vtkContourFilter.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkProbeFilter.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkScalarBarActor.h"
#include "vtkTextProperty.h"
#include "vtkTextMapper.h"
#include "vtkActor2D.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#define VTK_CREATE( type, var ) vtkSmartPointer< type > var = vtkSmartPointer< type >::New()

int main() {
	VTK_CREATE( vtkImageData, img );
	img->SetDimensions( 21, 21, 21 );
	img->SetExtent( 0, 20, 0, 20, 0, 20 );
	img->SetSpacing( 1, 1, 1 );
	img->SetOrigin( 0, 0, 0 );
	img->AllocateScalars( VTK_FLOAT, 1 );

	VTK_CREATE( vtkImageData, base );
	base->SetDimensions( 21, 21, 21 );
	base->SetSpacing( 1, 1, 1 );
	base->SetOrigin( 0, 0, 0 );
	base->AllocateScalars( VTK_UNSIGNED_SHORT, 1 );

	VTK_CREATE( vtkUnsignedShortArray, pBaseArr );
	pBaseArr->SetNumberOfValues( 21 * 21 * 21 );

	VTK_CREATE( vtkFloatArray, pImgArr );
	pImgArr->SetNumberOfValues( 21 * 21 * 21 );



	float sigma = 5.0;
	for( int x = 0; x < 21; x++ )
	for( int y = 0; y < 21; y++ )
	for( int z = 0; z < 21; z++ )
	{
		double valX = exp( (double)( ( x - 11 ) * ( x - 11 ) ) / (double)( 2.0 * M_PI * sigma * sigma ) ) / (double)( sqrt( 2.0 * M_PI ) * sigma ) ;
		double valY = exp( (double)( ( y - 11 ) * ( y - 11 ) ) / (double)( 2.0 * M_PI * sigma * sigma ) ) / (double)( sqrt( 2.0 * M_PI ) * sigma ) ;
		double valZ = exp( (double)( ( z - 11 ) * ( z - 11 ) ) / (double)( 2.0 * M_PI * sigma * sigma ) ) / (double)( sqrt( 2.0 * M_PI ) * sigma ) ;
		double val = valX * valY * valZ * 2000;

//		img->SetScalarComponentFromFloat( x, y, z, 0, val );
//		base->SetScalarComponentFromFloat( x, y, z, 0, 255 );

		int idx = x + y * 21 + z * 21 * 21;

		pBaseArr->SetValue( idx, 255 );
		pImgArr->SetValue( idx, val );

		cout << val << '\n';
	}
	pBaseArr->Modified();
	pImgArr->Modified();

	img->GetPointData()->SetScalars( pImgArr );
	base->GetPointData()->SetScalars( pBaseArr );

	img->Modified();
	base->Modified();

 	// Colorize Distance Image
 	VTK_CREATE( vtkContourFilter, pFilter );
 	pFilter->SetInputData( base );
 	pFilter->ComputeScalarsOn();
 	pFilter->SetValue( 0, 255 );
 	pFilter->Update();

 	VTK_CREATE( vtkLookupTable, pLUT );
 	pLUT->SetTableRange( -5, 5 );
 	pLUT->SetHueRange( 0.0, 0.666 );
 	pLUT->SetValueRange( 1.0, 1.0 );
 	pLUT->SetSaturationRange( 1.0, 1.0 );
 	pLUT->SetAlphaRange( 1.0, 1.0 );
 	pLUT->Build();

 	VTK_CREATE( vtkImageMapToColors, pMapColor );
 	pMapColor->SetInputData( img );
 	pMapColor->SetLookupTable( pLUT );
 	pMapColor->SetOutputFormatToRGB();
 	pMapColor->Update();

 	vtkImageData* colorImg = pMapColor->GetOutput();



 	VTK_CREATE( vtkProbeFilter, pProbe );
 	pProbe->SetInputConnection( pFilter->GetOutputPort() );
 	pProbe->SetSourceConnection( pMapColor->GetOutputPort() );
 	pProbe->Update();

 	// Smooothing
 	VTK_CREATE( vtkSmoothPolyDataFilter, pSmooth );
 	pSmooth->SetInputConnection( pProbe->GetOutputPort() );
 	pSmooth->SetNumberOfIterations( 50 );
 	pSmooth->Update();

 	VTK_CREATE( vtkPolyDataMapper, pMapper );
// 	pMapper->SetInputConnection( pSmooth->GetOutputPort() );
 	pMapper->SetInputConnection( pProbe->GetOutputPort() );
 	pMapper->Update();

 	// Show error range using a scalar bar.
 	VTK_CREATE( vtkScalarBarActor, pBarActor );
 	pBarActor->SetLookupTable( pLUT );
 	pBarActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
 	pBarActor->GetPositionCoordinate()->SetValue( 0.1, 0.05 );
 	pBarActor->SetOrientationToVertical();
 	pBarActor->SetWidth( 0.1 );
 	pBarActor->SetHeight( 0.9 );
 	pBarActor->SetPosition( 0.01, 0.1 );
 	pBarActor->SetNumberOfLabels( 0 );

 	VTK_CREATE( vtkTextProperty, pTextProp );
 	pTextProp->SetFontSize( 10 );
 	pTextProp->SetFontFamilyToCourier();
 	pTextProp->SetBold( 0 );
 	pTextProp->SetColor( 0.3, 1, 1 );

 	VTK_CREATE( vtkTextMapper, pTextMapperTop );
 	pTextMapperTop->SetInput( "Positive\nMax" );
 	pTextMapperTop->SetTextProperty( pTextProp );

 	VTK_CREATE( vtkTextMapper, pTextMapperMid );
 	pTextMapperMid->SetInput( "Zero" );
 	pTextMapperMid->SetTextProperty( pTextProp );

 	VTK_CREATE( vtkTextMapper, pTextMapperBot );
 	pTextMapperBot->SetInput( "Negative\nMax" );
 	pTextMapperBot->SetTextProperty( pTextProp );

 	VTK_CREATE( vtkActor2D, pTextActorTop );
 	pTextActorTop->SetMapper( pTextMapperTop );
 	pTextActorTop->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
 	pTextActorTop->GetPositionCoordinate()->SetValue( 0.1, 0.05 );
 	pTextActorTop->SetPosition( 0.11, 0.85 );

 	VTK_CREATE( vtkActor2D, pTextActorMid );
 	pTextActorMid->SetMapper( pTextMapperMid );
 	pTextActorMid->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
 	pTextActorMid->GetPositionCoordinate()->SetValue( 0.1, 0.05 );
 	pTextActorMid->SetPosition( 0.11, 0.46 );

 	VTK_CREATE( vtkActor2D, pTextActorBot );
 	pTextActorBot->SetMapper( pTextMapperBot );
 	pTextActorBot->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
 	pTextActorBot->GetPositionCoordinate()->SetValue( 0.1, 0.05 );
 	pTextActorBot->SetPosition( 0.11, 0.1 );

 	// Show Image
 	VTK_CREATE( vtkActor, pActor );
 	pActor->SetMapper( pMapper );

 	VTK_CREATE( vtkRenderer, pRenderer );
 	VTK_CREATE( vtkRenderWindow, pRenWin );
 	VTK_CREATE( vtkRenderWindowInteractor, pRenIn );
 	if( !pRenIn->GetInitialized() )
 	{
 		pRenWin->SetSize( 500, 500 );
 		pRenderer->ResetCamera();
 	}
 	pRenWin->AddRenderer( pRenderer );
 	pRenIn->SetRenderWindow( pRenWin );

 	pRenderer->SetBackground( 0, 0, 0 );
 	pRenderer->AddActor( pActor );
 	pRenderer->AddActor( pBarActor );
 	pRenderer->AddActor( pTextActorTop );
 	pRenderer->AddActor( pTextActorMid );
 	pRenderer->AddActor( pTextActorBot );

 	pRenWin->Render();
 	pRenIn->Initialize();
 	pRenIn->Start();

	return 0;
}