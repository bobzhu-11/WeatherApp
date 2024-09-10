// Fill out your copyright notice in the Description page of Project Settings.


#include "WeatherApp/Public/WeatherManager.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"


void UWeatherManager::GetWeatherByLocation(FString CityName)
{
	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest,ESPMode::ThreadSafe> Request = Http->CreateRequest();

	Request -> OnProcessRequestComplete().BindUObject(this,&UWeatherManager::OnWeatherResponseReceived);

	const FString ApiKey = TEXT("febb41a0dfb2155defa3cf556ec89716");
	const FString Url = FString::Printf(TEXT("http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s"), *CityName, *ApiKey);

	Request ->SetURL(Url);
	Request ->SetVerb("GET");
	Request->ProcessRequest();
	
	 
}

void UWeatherManager::OnWeatherResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response->GetResponseCode() == 200)
	{
		FString ResponseContent = Response -> GetContentAsString();

		UE_LOG(LogTemp,Log,TEXT("Weather Data: %s"), *ResponseContent);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get weather data!"));
	}
}


