-- The translate for emotions

return {
-- png emotions
	["assassinate-b"] = "决斗(攻)",
	["assassinate-a"] = "决斗(受)",
	["damage2"] = "伤害2",
	["win"] = "胜利",
	["lose"] = "失败",
	["victim"] = "受害者",
	["question"] = "质疑",
	["no-question"] = "不质疑",

	["^assassinate-b"] = "发动决斗者播放",
	["^assassinate-a"] = "被决斗者播放",
	["^damage2"] = "第二种伤害效果",
	["^win"] = "游戏结束时，胜利者播放",
	["^lose"] = "游戏结束时，失败者播放",
	["^question"] = "质疑",
	["^no-question"] = "不质疑",
	["^victim"] = "当你成为杀的目标时，头上出现的“好汉饶命”效果",
-- rcc emotions
	["awake"] = "觉醒效果",
	["limited"] = "限定效果",
	["chain"] = "铁索连环",
	["draw-card"] = "摸牌",
	["judgegood"] = "判定有利",
	["judgebad"] = "判定有害",
	["pindian"] = "拼点",
	["success"] = "拼点成功",
	["no-success"] = "拼点失败",
	["slash_red"] = "红杀",
	["slash_black"] = "黑杀",
	["fire_damage"] = "火焰伤害",
	["thunder_damage"] = "雷电伤害",
	["recover"] = "回复体力",
	["hplost"] = "体力流失",
	["killer"] = "打出杀",
	["death"] = "死亡",

	["^analeptic"] = "使用酒时播放",
	["^armor"] = "防具通用动画，触发防具效果时播放",
	["^awake"] = "当你发动觉醒技时播放",
	["^chain"] = "使用铁索连环时播放",
	["^damage"] = "受到普通伤害时播放",
	["^death"] = "阵亡时播放",
	["^draw-card"] = "从牌堆摸一张以上的牌时播放",
	["^duel"] = "决斗时播放",
	["^fire_damage"] = "受到火焰伤害时播放",
	["^fire_slash"] = "使用火杀时播放",
	["^horse"] = "装备坐骑时播放",
	["^hplost"] = "体力流失时播放",
	["^jink"] = "打出闪时播放",
	["^judgebad"] = "判定结果对判定者不利时播放",
	["^judgegood"] = "判定结果对判定者有利时播放",
	["^killer"] = "使用无色杀或响应猛虎下山打出杀时播放",
	["^lightning"] = "被闪电劈中时播放",
	["^limited"] = "当你发动限定技时播放",
	["^no-success"] = "拼点没赢时播放",
	["^peach"] = "使用肉时播放", -- 原来是可移动的，改为逐帧后失去了移动效果
	["^pindian"] = "发动拼点时播放",
	["^recover"] = "非桃酒影响的体力回复时播放",
	["^slash_black"] = "使用黑杀时播放",
	["^slash_red"] = "使用红杀时播放",
	["^success"] = "拼点赢时播放",
	["^thunder_damage"] = "受到雷电伤害时播放",
	["^thunder_slash"] = "使用雷杀时播放",
	["^tsunami"] = "被海啸席卷时播放",
	["^weapon"] = "武器通用动画，发动武器效果时播放",
}
