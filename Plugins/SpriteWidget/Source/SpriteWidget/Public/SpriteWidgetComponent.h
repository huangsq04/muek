// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "SpriteWidgetComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = "UserInterface", hidecategories = (Object, Activation, "Components|Activation", Sockets, Base, Lighting, LOD, Mesh), editinlinenew, meta = (BlueprintSpawnableComponent))
class SPRITEWIDGET_API USpriteWidgetComponent : public UWidgetComponent
{
	GENERATED_UCLASS_BODY()

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const;

	virtual bool ShouldDrawWidget() const override;

	virtual void BeginDestroy() override;
public:

	/** ����UI */
	UFUNCTION(BlueprintCallable, Category = "SpriteWidget")
	void UpdateUITexture(float Duration = 0.5f);

	/** �������ű���,Ĭ��Ϊ0.5 */
	UPROPERTY(EditAnywhere, Category = "SpriteWidget")
	float Scale;

	/** ������ʾ��� */
	UPROPERTY(EditAnywhere, Category = "SpriteWidget")
	float MaxDepth;

	/** ��С����ʾ��� */
	UPROPERTY(EditAnywhere, Category = "SpriteWidget")
	float MinDepth;

	/** ��̬UI */
	UPROPERTY(EditAnywhere, Category = "SpriteWidget")
	bool IsStaticUI;

	/** ��ȾUI��� */
	UPROPERTY(EditAnywhere, Category = "SpriteWidget")
	float MaxWidgetRenderDeltaTime = 0.5;

	/** ��������ʱ,MaxWidgetRenderDeltaTime��Ч,��Ҫ�ֶ�����UpdateUITexture()���� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteWidget")
	bool bIsPassiveUpdate = false;

	float WidgetRenderDeltaTime;

private:
	bool bIsExecutedUpdateUITexture = false;

	float UpdateUITexture_Duration = 0.0f;
};
