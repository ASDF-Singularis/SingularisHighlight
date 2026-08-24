#include "Components/SingularisHighlightComponent.h"

#include <Components/PrimitiveComponent.h>

USingularisHighlightComponent::USingularisHighlightComponent()
{
	SetIsReplicatedByDefault(false);

	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	bAutoActivate = false;
}

void USingularisHighlightComponent::BeginPlay()
{
	Super::BeginPlay();

	// 1) 解析并缓存高亮图元引用
	ResolvePrimitives();
}

void USingularisHighlightComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 1) 清空缓存的图元引用
	ResolvedPrimitives.Empty();

	Super::EndPlay(EndPlayReason);
}

void USingularisHighlightComponent::Enable()
{
	SetHighlighted(true);
}

void USingularisHighlightComponent::Disable()
{
	SetHighlighted(false);
}

void USingularisHighlightComponent::SetHighlighted(const bool IsHighlighted)
{
	// 1) 幂等：状态未变化时直接返回
	if (bIsHighlighted == IsHighlighted) return;
	bIsHighlighted = IsHighlighted;

	// 2) 广播高亮状态事件
	if (IsHighlighted)
		OnHighlightEnableEvent.Broadcast();
	else
		OnHighlightDisableEvent.Broadcast();

	// 3) 刷新全部图元的自定义深度渲染
	RefreshPrimitiveHighlight();
}

void USingularisHighlightComponent::ResolvePrimitives()
{
	ResolvedPrimitives.Empty();

	// 1) 遍历配置的组件引用，解析为有效的图元组件并缓存
	for (const auto& Reference : HighlightPrimitives)
	{
		UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Reference.GetComponent(GetOwner()));
		if (!IsValid(Primitive)) continue;

		ResolvedPrimitives.Add(Primitive);
	}
}

void USingularisHighlightComponent::RefreshPrimitiveHighlight()
{
	// 1) 按当前高亮状态刷新每个缓存图元的自定义深度渲染
	for (const TWeakObjectPtr<UPrimitiveComponent>& Primitive : ResolvedPrimitives)
	{
		if (!Primitive.IsValid()) continue;

		Primitive->SetRenderCustomDepth(bIsHighlighted);
		Primitive->SetCustomDepthStencilValue(bIsHighlighted ? CustomDepthStencilValue : 0);
	}
}
