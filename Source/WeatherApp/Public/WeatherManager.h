// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WeatherManager.generated.h"

class IHttpRequest;
class IHttpResponse;

UCLASS(Blueprintable)
class WEATHERAPP_API UWeatherManager : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable,Category = "Weather")
	void  GetWeatherByLocation(FString CityName);
private:
	void OnWeatherResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request,TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response, bool bWasSuccessful);
};
