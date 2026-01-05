// Fill out your copyright notice in the Description page of Project Settings.


#include "GeometryHelpers.h"

static FSplinePoint ExtractPointAtIndex(const USplineComponent* Spline, int32 Index)
{
	constexpr ESplineCoordinateSpace::Type C {ESplineCoordinateSpace::Local};
	return FSplinePoint{
		0.f,
		Spline->GetLocationAtSplinePoint(Index, C),
		Spline->GetArriveTangentAtSplinePoint(Index, C),
		Spline->GetLeaveTangentAtSplinePoint(Index, C),
		Spline->GetRotationAtSplinePoint(Index, C),
		Spline->GetScaleAtSplinePoint(Index),
	};
}

static FSplinePoint ExtractPointAtDistanceAlongSpline(const USplineComponent* Spline, float Distance)
{
	constexpr ESplineCoordinateSpace::Type C {ESplineCoordinateSpace::Local};
	const FVector Tangent {Spline->GetTangentAtDistanceAlongSpline(Distance,C)};
	return FSplinePoint{
		0.f,
		Spline->GetLocationAtDistanceAlongSpline(Distance, C),
		Tangent,
		Tangent,
		Spline->GetRotationAtDistanceAlongSpline(Distance, C),
		Spline->GetScaleAtDistanceAlongSpline(Distance),
	};
}

USplineComponent* UGeometryHelpers::CreateOffsettedSpline(USplineComponent* Dest, const USplineComponent* Source, FVector Offset, float CornerRadius)
{
	if (!IsValid(Dest))
		return Dest;
	ON_SCOPE_EXIT{Dest->UpdateSpline();};
	Dest->ClearSplinePoints(false);
	
	if (!IsValid(Source))
		return Dest;
	const int32 Count = Source->GetNumberOfSplinePoints();
	if (Count == 0)
		return Dest;

	auto AddPoint = [Dest, Key = 0](FSplinePoint Point) mutable
	{
		Point.InputKey = static_cast<float>(Key++);
		Dest->AddPoint(Point,false);
	};
	
	{
		FSplinePoint Floor = ExtractPointAtIndex(Source, 0);
		Floor.Rotation = Offset.Rotation() + Floor.ArriveTangent.Rotation();
		Floor.ArriveTangent = Floor.LeaveTangent = Offset;
		
		FSplinePoint LoopCut {Floor};
		const float OffsetMagnitude = Offset.Length();
		const float LoopCutMagnitude {OffsetMagnitude - CornerRadius};
		const FVector LoopCutOffset {Offset * (LoopCutMagnitude/OffsetMagnitude)};
		LoopCut.Position += LoopCutOffset;
		LoopCut.LeaveTangent*=(2.f*CornerRadius)/LoopCut.LeaveTangent.Length();
		
		FSplinePoint CurveStart{ExtractPointAtDistanceAlongSpline(Source, CornerRadius)};
		CurveStart.Position += Offset;
		CurveStart.ArriveTangent*=(2.f*CornerRadius)/CurveStart.ArriveTangent.Length();
		
		AddPoint(Floor);
		AddPoint(LoopCut);
		AddPoint(CurveStart);
	}

	{
		FSplinePoint Floor = ExtractPointAtIndex(Source, Count-1);
		Floor.Rotation = (-Offset).Rotation() + Floor.LeaveTangent.Rotation();
		Floor.ArriveTangent = Floor.LeaveTangent = -Offset;
		
		FSplinePoint LoopCut {Floor};
		const float OffsetMagnitude = Offset.Length();
		const float LoopCutMagnitude {OffsetMagnitude - CornerRadius};
		const FVector LoopCutOffset {Offset * (LoopCutMagnitude/OffsetMagnitude)};
		LoopCut.Position += LoopCutOffset;
		LoopCut.LeaveTangent*=(2.f*CornerRadius)/LoopCut.LeaveTangent.Length();
		
		FSplinePoint CurveEnd{ExtractPointAtDistanceAlongSpline(Source, Source->GetDistanceAlongSplineAtSplinePoint(Count-1)-CornerRadius)};
		CurveEnd.Position += Offset;
		CurveEnd.ArriveTangent*=(2.f*CornerRadius)/CurveEnd.ArriveTangent.Length();
		
		AddPoint(CurveEnd);
		AddPoint(LoopCut);
		AddPoint(Floor);
	}
	
	for (int32 i{1}; i !=Count ; ++i)
	{
		FSplinePoint Point = ExtractPointAtIndex(Source, i);
		Point.Position += Offset;
		AddPoint(Point);
	}
	return Dest;
}
