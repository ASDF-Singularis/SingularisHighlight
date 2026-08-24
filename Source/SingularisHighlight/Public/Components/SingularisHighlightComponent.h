#pragma once

#include <CoreMinimal.h>
#include <Components/ActorComponent.h>

#include "SingularisHighlightComponent.generated.h"

class UPrimitiveComponent;

#pragma region 委托签名

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHighlightEnableSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHighlightDisableSignature);

#pragma endregion

/**
 * 引力奇点高亮组件。
 *
 * 通过 CustomDepth 自定义深度通道为指定图元提供高亮反馈，常用于交互目标指示、选中反馈等场景。
 * BeginPlay 时将高亮图元引用解析并缓存；Enable/Disable 幂等地切换高亮状态并广播事件。
 * 高亮为纯视觉表现，不做网络复制。
 */
UCLASS(
	Blueprintable,
	BlueprintType,
	ClassGroup = ("Singularis"),
	meta = (BlueprintSpawnableComponent, DisplayName = "引力奇点高亮组件")
)
class SINGULARISHIGHLIGHT_API USingularisHighlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:
#pragma region Parameter

	/**
	 * 高亮图元组件引用集合。
	 *
	 * 指向 Owner 上需要高亮渲染的 UPrimitiveComponent，支持多选组件引用。
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "SingularisHighlight|引力奇点高亮组件|参数",
		meta = (DisplayName = "高亮图元", UseComponentPicker, AllowedClasses = "/Script/Engine.PrimitiveComponent")
	)
	TArray<FComponentReference> HighlightPrimitives{};

	/**
	 * 高亮时写入图元的自定义深度模板值。
	 *
	 * 需与材质自定义深度模板读取条件（如 Stencil 比较值）配合使用，非高亮时写回 0。
	 */
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "SingularisHighlight|引力奇点高亮组件|参数",
		meta = (DisplayName = "自定义深度模板值")
	)
	int32 CustomDepthStencilValue = 1;

#pragma endregion

#pragma region 事件分发器

	/** 高亮启用时广播 */
	UPROPERTY(
		BlueprintAssignable,
		Category = "SingularisHighlight|引力奇点高亮组件|事件分发器",
		meta = (DisplayName = "高亮启用时触发")
	)
	FOnHighlightEnableSignature OnHighlightEnableEvent{};

	/** 高亮禁用时广播 */
	UPROPERTY(
		BlueprintAssignable,
		Category = "SingularisHighlight|引力奇点高亮组件|事件分发器",
		meta = (DisplayName = "高亮禁用时触发")
	)
	FOnHighlightDisableSignature OnHighlightDisableEvent{};

#pragma endregion

private:
#pragma region Internal Variable

	/** 当前高亮状态 */
	bool bIsHighlighted = false;

	/** BeginPlay 时解析并缓存的高亮图元 */
	TArray<TWeakObjectPtr<UPrimitiveComponent>> ResolvedPrimitives{};

#pragma endregion

public:
#pragma region Constructors

	USingularisHighlightComponent();

#pragma endregion

#pragma region ActorComponent Interface

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#pragma endregion

#pragma region State

	/**
	 * 当前是否处于高亮状态。
	 *
	 * @return 高亮启用时返回 true。
	 */
	UFUNCTION(
		BlueprintPure,
		BlueprintCallable,
		Category = "SingularisHighlight|引力奇点高亮组件|State",
		meta = (DisplayName = "IsHighlighted")
	)
	bool IsHighlighted() const { return bIsHighlighted; }

#pragma endregion

#pragma region API

	/**
	 * 启用高亮。幂等，状态已为高亮时直接返回。
	 */
	UFUNCTION(
		BlueprintCallable,
		Category = "SingularisHighlight|引力奇点高亮组件|API",
		meta = (DisplayName = "Enable")
	)
	void Enable();

	/**
	 * 禁用高亮。幂等，状态已为非高亮时直接返回。
	 */
	UFUNCTION(
		BlueprintCallable,
		Category = "SingularisHighlight|引力奇点高亮组件|API",
		meta = (DisplayName = "Disable")
	)
	void Disable();

#pragma endregion

private:
#pragma region Internal Function

	/**
	 * 统一的状态设置入口，供外部系统直接驱动高亮。
	 * 状态未变化时幂等返回，状态变化后广播对应事件并刷新图元渲染。
	 */
	void SetHighlighted(bool IsHighlighted);

	/**
	 * 解析 HighlightPrimitives 组件引用并缓存到 ResolvedPrimitives。
	 * 在 BeginPlay 中调用。
	 */
	void ResolvePrimitives();

	/**
	 * 按当前高亮状态刷新全部缓存图元的自定义深度渲染。
	 * 高亮时写入 CustomDepthStencilValue，非高亮时关闭自定义深度并清零模板值。
	 */
	void RefreshPrimitiveHighlight();

#pragma endregion
};
