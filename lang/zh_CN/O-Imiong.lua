-- translation for Imiong

return {
	["imiong"] = "阴阳",
	["conjuring"] = "咒术",
	["conjur_sys"] = "咒术系统",
	
	["moon"] = "阴",
	["sun"] = "阳",
	["multiply"] = "×",

	["poison_jur"] = "中毒",
	[":poison_jur"] = "目标角色回合开始时，随机弃置一张牌；第4回合转变为“剧毒”（目标角色回合开始时，该角色需选择一项：随机弃置两张牌，或失去1点体力）。附加概率100%，共持续5个回合。",
	["sleep_jur"] = "昏睡",
	[":sleep_jur"] = "目标角色于其回合外不能使用或打出牌，于其回合内跳过出牌阶段，直到其受到一次伤害。附加概率75%，共持续2个回合。",
	["dizzy_jur"] = "晕眩",
	[":dizzy_jur"] = "目标角色不能发动其当前的所有技能，直到其进入濒死状态。附加概率75%，共持续2个回合。",
	["stealth_jur"] = "隐形",
	[":stealth_jur"] = "目标角色不能成为任何卡牌指定的目标。共持续2个回合。",
	["chaos_jur"] = "混乱",
	[":chaos_jur"] = "目标角色使用【杀】、【决斗】或【行刺】时，该【杀】、【决斗】或【行刺】有75%的几率随机转移给另一名目标角色（无距离限制）。共持续2个回合。",
	["weak_jur"] = "虚弱",
	["mildew_jur"] = "霉运",
	["reflex_jur"] = "反弹",
	[":reflex_jur"] = "目标角色受到伤害时，该角色有25%的几率将该伤害转移给伤害来源。共持续3个回合。",
	["cure_jur"] = "回复",
	["revolt_jur"] = "抵抗",
	[":revolt_jur"] = "目标角色附加所有咒术状态的几率-50%。共持续5个回合。",
	["violent_jur"] = "狂暴",
	["lucky_jur"] = "幸运",
	[":lucky_jur"] = "目标角色摸牌阶段，该角色有75%的几率额外摸一张牌。共持续2个回合。",
	["invalid_jur"] = "无效",

	["#ForbidJur"] = "咒术附加失败！%from 未被附加 %arg 状态",
	["#UntriggerJur"] = "咒术失效！%from 的 %arg 状态 未产生任何影响",
	["#GainJur"] = "%from 进入了 %arg 状态",
	["#RemoveJur"] = "%from 解除了 %arg 状态",
	["#Poison"] = "%from 毒发！剧痛难忍，将自己抓出了条条血痕",
	["poison_jur:cd"] = "丢两张牌",
	["poison_jur:hp"] = "掉一点体力",
	["#Sleep"] = "%from 在昏睡，无法使用或打出牌！",
	["#Lucky"] = "%from 被幸福敲门，额外摸了1张牌 ^_^",
	["#Chaos"] = "%from 陷入混乱，竟然向 %to 发动了攻击！",
}
