// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonObjectLoader.generated.h"


USTRUCT(BlueprintType)
struct FLevelObject
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Level Object")
    FString Name;

    UPROPERTY(BlueprintReadWrite, Category = "Level Object")
    FString ID;

    UPROPERTY(BlueprintReadWrite, Category = "Level Object")
    int32 Type;

    UPROPERTY(BlueprintReadWrite, Category = "Level Object")
    FVector Position;

    UPROPERTY(BlueprintReadWrite, Category = "Level Object")
    FRotator Rotation;

    UPROPERTY(BlueprintReadWrite, Category = "Level Object")
    FVector Scale;

    UPROPERTY(BlueprintReadWrite, Category = "Level Object")
    int32 PhysicsEnabled;
};


UCLASS()
class GDENG03_FINALS_API UJsonObjectLoader : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Level Parser")
	static TArray<FLevelObject> ParseLevelFile(const FString& filePath);

    UFUNCTION(BlueprintCallable, Category = "Level File")
    static void ExportLevelFile(const TArray<AActor*>& actors, const FString& fileName);
};
