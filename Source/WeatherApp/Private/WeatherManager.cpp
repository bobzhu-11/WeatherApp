// Fill out your copyright notice in the Description page of Project Settings.


#include "WeatherApp/Public/WeatherManager.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"


void UWeatherManager::GetWeatherByLocation(FString CityName)
{
	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnWeatherResponseReceived);

	const FString ApiKey = TEXT("febb41a0dfb2155defa3cf556ec89716");
	const FString Url = FString::Printf(
		TEXT("http://api.openweathermap.org/data/2.5/forecast?q=%s&appid=%s"), *CityName, *ApiKey);

	Request->SetURL(Url);
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

void UWeatherManager::GetUserLocation()
{
	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnLocationResponseReceived);
	const FString Url = TEXT("http://ip-api.com/json/");
	Request->SetURL(Url);
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

FString UWeatherManager::GetCachedLocation() const
{
	return *CachedLocation;
}

void UWeatherManager::OnLocationResponseReceived(FHttpRequestPtr Request,
                                                 FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response->GetResponseCode() == 200)
	{
		FString ResponseContent = Response->GetContentAsString();

		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			CachedLocation = JsonObject->GetStringField("city");
			UE_LOG(LogTemp, Log, TEXT("Current City: %s"), *CachedLocation);
			OnLocationUpdated.Broadcast();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to parse location JSON data."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get weather data!"));
	}
}

void UWeatherManager::OnWeatherResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response->GetResponseCode() == 200)
	{
		FString ResponseContent = Response->GetContentAsString();

		UE_LOG(LogTemp, Log, TEXT("Weather Data: %s"), *ResponseContent);

		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			const TSharedPtr<FJsonObject>* MainObject;
			if (JsonObject->TryGetObjectField(TEXT("main"), MainObject))
			{
				if (JsonObject->TryGetObjectField(TEXT("main"), MainObject))
				{
					float Temperature = ((*MainObject)->GetNumberField(TEXT("temp")) - 273.15f);
					UE_LOG(LogTemp, Log, TEXT("Temperature: %f ℃"), Temperature);
				}
				
				const TArray<TSharedPtr<FJsonValue>>* WeatherArray;
				if (JsonObject->TryGetArrayField(TEXT("weather"), WeatherArray))
				{
					if ((*WeatherArray).Num() > 0)
					{
						TSharedPtr<FJsonObject> WeatherObject = (*WeatherArray)[0]->AsObject();
						FString Description = WeatherObject->GetStringField(TEXT("description"));
						UE_LOG(LogTemp, Log, TEXT("Weather: %s"), *Description);
					}
				}
			}
			OnWeatherUpdated.Broadcast();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get weather data!"));
	}
}
