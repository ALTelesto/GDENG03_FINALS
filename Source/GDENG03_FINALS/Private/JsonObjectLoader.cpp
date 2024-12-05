// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonObjectLoader.h"

// unreal's built-in json blueprint stuff sucks so let's do plain text reading instead
TArray<FLevelObject> UJsonObjectLoader::ParseLevelFile(const FString& filePath)
{
    TArray<FLevelObject> parsedObjects;
    FString content;

    if (!FFileHelper::LoadFileToString(content, *filePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load level file: %s"), *filePath);
        return parsedObjects;
    }

    TArray<FString> lines;
    content.ParseIntoArrayLines(lines);

    FLevelObject currentObject;

    for (int32 i = 0; i < lines.Num(); i++)
    {
        const FString& line = lines[i];

        if (!line.IsEmpty() && !line.StartsWith(TEXT("Type:")) &&
            !line.StartsWith(TEXT("Position:")) &&
            !line.StartsWith(TEXT("Rotation:")) &&
            !line.StartsWith(TEXT("Scale:")) &&
            !line.StartsWith(TEXT("Physics Enabled:")))
        {
            if (!currentObject.Name.IsEmpty())
            {
                parsedObjects.Add(currentObject);
            }

            currentObject = FLevelObject();
            currentObject.Name = line;
            currentObject.ID = "lazy"; // we dont need this so i get to be lazy
            i++;
        }
        else if (line.StartsWith(TEXT("Type:")))
        {
            currentObject.Type = FCString::Atoi(*line.Mid(5).TrimStart());
        }
        else if (line.StartsWith(TEXT("Position:")))
        {
            FString positionData = line.Mid(9).TrimStart();
            TArray<FString> positionValues;
            positionData.ParseIntoArray(positionValues, TEXT(" "));
            if (positionValues.Num() == 3)
            {
                // why are you xzy unreal
                currentObject.Position.X = FCString::Atof(*positionValues[0]);
                currentObject.Position.Y = -(FCString::Atof(*positionValues[2]));
                currentObject.Position.Z = FCString::Atof(*positionValues[1]);
            }
        }
        else if (line.StartsWith(TEXT("Rotation:")))
        {
            FVector rotationRadians;
            FString rotationData = line.Mid(9).TrimStart();
            /*FVector temp = { -90,0,0 }; // attempt at fixing rotation
            temp = FMath::DegreesToRadians(temp);*/
            TArray<FString> rotationValues;
            rotationData.ParseIntoArray(rotationValues, TEXT(" "));
            if (rotationValues.Num() == 3)
            {
                // why are you xzy unreal
                rotationRadians.X = FCString::Atof(*rotationValues[0]);
                rotationRadians.Y = FCString::Atof(*rotationValues[2]);
                rotationRadians.Z = FCString::Atof(*rotationValues[1]);

                FVector rotationDegrees = FMath::RadiansToDegrees(rotationRadians);
                currentObject.Rotation = FRotator(rotationDegrees.X, rotationDegrees.Y, rotationDegrees.Z);
            }
        }
        else if (line.StartsWith(TEXT("Scale:")))
        {
            FString scaleData = line.Mid(6).TrimStart();
            TArray<FString> scaleValues;
            scaleData.ParseIntoArray(scaleValues, TEXT(" "));
            if (scaleValues.Num() == 3)
            {
                // why are you xzy unreal
                currentObject.Scale.X = FCString::Atof(*scaleValues[0]);
                currentObject.Scale.Y = FCString::Atof(*scaleValues[2]);
                currentObject.Scale.Z = FCString::Atof(*scaleValues[1]);
            }
        }
        else if (line.StartsWith(TEXT("Physics Enabled:")))
        {
            currentObject.PhysicsEnabled = FCString::Atoi(*line.Mid(16).TrimStart());
        }
    }
    if (!currentObject.Name.IsEmpty())
    {
        parsedObjects.Add(currentObject);
    }

    return parsedObjects;
}

void UJsonObjectLoader::ExportLevelFile(const TArray<AActor*>& actors, const FString& fileName)
{
    FString fileContent;
    int32 id = 0;
    for (AActor* actor : actors)
    {
        if (!actor) continue;

        FString name = actor->GetActorLabel();

        FVector position = actor->GetActorLocation();
        FRotator rotation = actor->GetActorRotation();
        FVector scale = actor->GetActorScale3D();

        int32 type = 0;
        if (actor->Tags.Contains(TEXT("Cube"))) type = 1;
        else if (actor->Tags.Contains(TEXT("Plane"))) type = 2;
        else if (actor->Tags.Contains(TEXT("Sphere"))) type = 3;
        else if (actor->Tags.Contains(TEXT("Capsule"))) type = 4;

        // we ignore non compliance!!!!
        if(type != 0)
	    {
        	int32 physicsEnabled = 0;
        	UStaticMeshComponent* meshComponent = actor->FindComponentByClass<UStaticMeshComponent>();
        	if (meshComponent && meshComponent->IsSimulatingPhysics())
        	{
        		physicsEnabled = 1;
        	}
            //because i doubled the capsule scale for accuracy, i need to divide it here
            if(type == 4)
            {
                scale /= 2;
            }
            //because i multiplied the plane scale for accuracy, i need to divide it here too
            else if(type == 2)
            {
                scale /= 10;
            }

        	FString objectData = FString::Printf(
				TEXT("%s\n%d\nType: %d\nPosition: %.2f %.2f %.2f\nRotation: %.2f %.2f %.2f\nScale: %.2f %.2f %.2f\nPhysics Enabled: %d\n"),
				*name,
				id++,
				type,
				position.X/100, position.Z/100, -position.Y/100, // 1:100 conversion ratio unity:unreal
				FMath::DegreesToRadians(rotation.Pitch),
				FMath::DegreesToRadians(rotation.Yaw),
				FMath::DegreesToRadians(rotation.Roll),
				scale.X, scale.Z, scale.Y,
				physicsEnabled
			);

        	fileContent.Append(objectData);
	    }
    }
    FString filePath = FPaths::ProjectDir() + fileName + ".level";
    if (FFileHelper::SaveStringToFile(fileContent, *filePath))
    {
        UE_LOG(LogTemp, Log, TEXT("saved as: %s"), *filePath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("hoy you failed to save: %s"), *filePath);
    }
}
