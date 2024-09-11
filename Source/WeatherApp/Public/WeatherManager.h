// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WeatherManager.generated.h"

class IHttpRequest;
class IHttpResponse;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeatherUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLocationUpdated);

USTRUCT(BlueprintType)
struct FWeatherInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Weather")
	float Temp;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Weather")
	float TempMin;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Weather")
	float TempMax;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Weather")
	FString Dates;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Weather")
	FString Icon;
};
UCLASS(Blueprintable)
class WEATHERAPP_API UWeatherManager : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Location")
	void GetWeatherByLocation(FString CityName);
	UPROPERTY(BlueprintAssignable, Category = "Location")
	FOnLocationUpdated OnLocationUpdated;
	UFUNCTION(BlueprintCallable, Category = "Location")
	FString GetCachedLocation() const;
	UFUNCTION(BlueprintCallable, Category="Weather")
	void GetUserLocation();
	UPROPERTY(BlueprintAssignable, Category = "Weather")
	FOnWeatherUpdated OnWeatherUpdated;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Weather")
	TArray<FWeatherInfo> WeatherInfos;

private:
	void OnLocationResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request,
								   TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response, bool bWasSuccessful);
	void OnWeatherResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request,
	                               TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response, bool bWasSuccessful);
	FString CachedLocation= TEXT("");
};
