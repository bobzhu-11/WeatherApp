// Fill out your copyright notice in the Description page of Project Settings.


#include "WeatherApp/Public/WeatherManager.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"


void UWeatherManager::GetCurrentWeatherByLocation(FString CityName)
{
	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnCurrentWeatherResponseReceived);

	const FString ApiKey = TEXT("febb41a0dfb2155defa3cf556ec89716");
	const FString Url = FString::Printf(
		TEXT("http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s"), *CityName, *ApiKey);

	Request->SetURL(Url);
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

void UWeatherManager::GetWeatherForecastByLocation(FString CityName)
{
	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnWeatherForecastResponseReceived);

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
			UserLocationInfo.Country = JsonObject->GetStringField(TEXT("country"));
			UserLocationInfo.City = JsonObject->GetStringField(TEXT("city"));
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

void UWeatherManager::OnCurrentWeatherResponseReceived(FHttpRequestPtr Request,
                                                       FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response->GetResponseCode() == 200)
	{
		FString ResponseContent = Response->GetContentAsString();
		UE_LOG(LogTemp, Log, TEXT("Current Weather Data: %s"), *ResponseContent);
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			TSharedPtr<FJsonObject> Main = JsonObject->GetObjectField(TEXT("main"));
			TSharedPtr<FJsonObject> Sys = JsonObject->GetObjectField(TEXT("sys"));
			TArray<TSharedPtr<FJsonValue>> WeatherArray = JsonObject->GetArrayField(TEXT("weather"));
			TSharedPtr<FJsonObject> WeatherObject = WeatherArray[0]->AsObject();

			CurrentWeatherInfo.Temp = (Main->GetNumberField(TEXT("temp"))) - 273.15f;
			CurrentWeatherInfo.TempMin = (Main->GetNumberField(TEXT("temp_min"))) - 273.15f;
			CurrentWeatherInfo.TempMax = (Main->GetNumberField(TEXT("temp_max"))) - 273.15f;
			CurrentWeatherInfo.FeelsLike = (Main->GetNumberField(TEXT("feels_like"))) - 273.15f;
			CurrentWeatherInfo.Pressure = Main->GetNumberField(TEXT("pressure"));
			CurrentWeatherInfo.Humidity = Main->GetNumberField(TEXT("humidity"));
			CurrentWeatherInfo.Visibility = JsonObject->GetNumberField(TEXT("visibility"));
			CurrentWeatherInfo.Weather = WeatherObject->GetStringField(TEXT("main"));
			CurrentWeatherInfo.Icon = WeatherObject->GetStringField(TEXT("icon"));
			CurrentWeatherInfo.Country = Sys->GetStringField(TEXT("country"));
			CurrentWeatherInfo.Sunrise = Sys->GetNumberField(TEXT("sunrise"));
			CurrentWeatherInfo.Sunset = Sys->GetNumberField(TEXT("sunset"));

			OnCurrentWeatherUpdated.Broadcast();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get weather data!"));
	}
}

void UWeatherManager::OnWeatherForecastResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response,
                                                        bool bWasSuccessful)
{
	if (bWasSuccessful && Response->GetResponseCode() == 200)
	{
		FString ResponseContent = Response->GetContentAsString();

		UE_LOG(LogTemp, Log, TEXT("Weather Forecast Data: %s"), *ResponseContent);

		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			const TArray<TSharedPtr<FJsonValue>>& List = JsonObject->GetArrayField(TEXT("list"));
			for (int32 i = 0; i < List.Num(); i++)
			{
				FWeatherInfo WeatherForecastInfo;
				TSharedPtr<FJsonObject> ListItem = List[i]->AsObject();
				TSharedPtr<FJsonObject> Main = ListItem->GetObjectField(TEXT("main"));
				TArray<TSharedPtr<FJsonValue>> WeatherArray = ListItem->GetArrayField(TEXT("weather"));
				TSharedPtr<FJsonObject> WeatherObject = WeatherArray[0]->AsObject();
				
				WeatherForecastInfo.Temp = (Main->GetNumberField(TEXT("temp"))) - 273.15f;
				WeatherForecastInfo.Weather = WeatherObject->GetStringField(TEXT("main"));
				WeatherForecastInfo.Icon = WeatherObject->GetStringField(TEXT("icon"));
				WeatherForecastInfo.Dates = ListItem->GetStringField(TEXT("dt_txt"));

				WeatherForecastInfos.Add(WeatherForecastInfo);
			}
			OnWeatherForecastUpdated.Broadcast();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get weather forecast data!"));
	}
}
