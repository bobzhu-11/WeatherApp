// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WeatherManager.generated.h"

class IHttpRequest;
class IHttpResponse;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLocationUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeatherUpdated);

USTRUCT(BlueprintType)
struct FWeatherInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weather")
	float Temp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weather")
	float TempMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weather")
	float TempMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weather")
	FString Dates;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weather")
	FString Icon;
};

UCLASS(Blueprintable)
class WEATHERAPP_API UWeatherManager : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Weather")
	void GetCurrentWeatherByLocation(FString CityName);
	UFUNCTION(BlueprintCallable, Category = "Weather")
	void GetWeatherForecastByLocation(FString CityName);
	UPROPERTY(BlueprintAssignable, Category = "Weather")
	FOnWeatherUpdated OnCurrentWeatherUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Weather")
	FOnWeatherUpdated OnWeatherForecastUpdated;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weather")
	FWeatherInfo CurrentWeatherInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weather")
	TArray<FWeatherInfo> WeatherForecastInfos;
	UPROPERTY(BlueprintAssignable, Category = "Location")
	FOnLocationUpdated OnLocationUpdated;
	UFUNCTION(BlueprintCallable, Category = "Location")
	FString GetCachedLocation() const;
	UFUNCTION(BlueprintCallable, Category="Location")
	void GetUserLocation();

private:
	void OnLocationResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request,
	                                TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response, bool bWasSuccessful);
	void OnCurrentWeatherResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request,
	                                      TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response, bool bWasSuccessful);
	void OnWeatherForecastResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request,
	                                       TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response,
	                                       bool bWasSuccessful);
	FString CachedLocation = TEXT("");
};
