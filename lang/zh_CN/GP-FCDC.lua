-- FengchiDianche Shuihusha part 9.

return {
	["FCDC"] = "风驰电掣",

	["#huyanzhuo"] = "双鞭",
	["huyanzhuo"] = "呼延灼",
	["coder:huyanzhuo"] = "Slob",
	["lianma"] = "链马",
	[":lianma"] = "出牌阶段，你可以将你的任一马匹当【铁锁连环】使用。",
	["lianma:lian"] = "链起来！",
	["lianma:ma"] = "解链！",
	["zhongjia"] = "重甲",
	[":zhongjia"] = "<b>锁定技</b>，你的手牌上限+X（X为处于连环状态的现存角色数）。",

	["#dongping"] = "双枪将",
	["dongping"] = "董平",
	["coder:dongping"] = "战栗贵公子",
	["shuangzhan"] = "双战",
	[":shuangzhan"] = "若你攻击范围内的现存其他角色数大于2，则你使用【杀】时可以额外指定一名目标角色；当你使用【杀】指定一名其他角色为目标后，若你攻击范围内的现存其他角色数不大于2，则该角色需连续使用两张【闪】才能抵消。",
	["@shuangzhan-jink-1"] = "%src 拥有【双战】技能，您必须连续出两张【闪】",
	["@shuangzhan-jink-2"] = "%src 拥有【双战】技能，你还需出一张【闪】",
	["$shuangzhan1"] = "一对白龙争上下！",-- 1和2效果一发动
	["$shuangzhan2"] = "银蟒花人眼，双枪夺命归！",
	["$shuangzhan3"] = "董一撞在此！", -- 3和4效果二发动
	["$shuangzhan4"] = "匹夫，受死！",
	["~dongping"] = "背后有人？",

	["#shixiu"] = "拼命三郎",
	["shixiu"] = "石秀",
	["designer:shixiu"] = "天尸娘",
	["boming"] = "搏命",
	[":boming"] = "出牌阶段，你可以对一名攻击范围内的其他角色造成X点伤害，X为该角色装备区的装备数目，然后你流失X点体力，每阶段限一次",
	["~shixiu"] = "箭如雨下，吾命休矣！",

	["#xiezhen"] = "两头蛇",
	["xiezhen"] = "解珍",
	["cv:xiezhen"] = "烨子【剪刀剧团】",
	["xunlie"] = "巡猎",
	[":xunlie"] = "你可以弃置一张装备牌跳过你的摸牌阶段，获得除你外手牌数最多的一名角色的至多两张手牌（至少一张）。若你以此法获得的牌为一张，你摸一张牌。",
	["@xunlie"] = "你可以跳过摸牌阶段，发动【巡猎】",
	["get"] = "纳尼，还有牌？再拿一张！",
	["cancel"] = "不拿了",
	["$xunlie1"] = "金蛇弓响，虎狼难逃。",
	["$xunlie2"] = "吾这便去取讨大虫！",
	["~xiezhen"] = "顾不了那么多了！",

	["#pangwanchun"] = "小养由基",
	["pangwanchun"] = "庞万春",
	["shenjian"] = "神箭",
	[":shenjian"] = "<b>锁定技</b>，【万箭齐发】对你无效；当你使用【万箭齐发】时，目标角色须连续打出两张【闪】才能抵消。",
	["#Shenjian"] = "%from 的锁定技【%arg】被触发，%to 需要打出两张【闪】",
	["@shenjian2jink"] = "%src 拥有技能【神箭】，你须再出一张【闪】",
	["lianzhu"] = "连珠",
	[":lianzhu"] = "出牌阶段。你可将你的武将牌翻面，视为你使用了一张【万箭齐发】。每回合限一次。",
	["~pangwanchun"] = "中埋伏了……",

	["#jiangjing"] = "神算子",
	["jiangjing"] = "蒋敬",
	["tiansuan"] = "天算",
	[":tiansuan"] = "每当两名角色进行拼点时（在双方的拼点牌亮出前），你可以观看一次双方拼点的牌，并可以将其中一张与牌堆顶第一张牌进行交换。",
	["@tiansuan"] = "%src 的拼点牌是 %dest %arg 的 %arg2",
	["huazhu"] = "划珠",
	[":huazhu"] = "出牌阶段，你可以和一名角色拼点，若你赢，你须令任一角色将手牌调整至X张，X为你拼点牌点数的一半（向上取整），每阶段限一次。",

	["#ruanxiaowu"] = "短命二郎",
	["ruanxiaowu"] = "阮小五",
	["designer:ruanxiaowu"] = "hypercross",
	["shuilao"] = "水牢",
	[":shuilao"] = "出牌阶段，你可以将你的任一非延时锦囊牌当【画地为牢】使用。",
	["shuizhan"] = "水遁",
	[":shuizhan"] = "锁定技，其他角色计算相互距离时，跳过你；其他角色计算与你的距离时，至少为2",
	["~ruanxiaowu"] = "大江东去浪淘尽。",

	["#maling"] = "神驹子",
	["maling"] = "马灵",
	["fengxing"] = "风行",
	[":fengxing"] = "<b>锁定技</b>，你始终跳过你的判定阶段、摸牌阶段和弃牌阶段；你每失去一张手牌，若你的手牌数小于你的体力上限，则你须摸一张牌。",

	["#daizong"] = "神行太保",
	["daizong"] = "戴宗",
	["mitan"] = "密探",
	[":mitan"] = "你可以将你的任一锦囊牌或事件牌当【探听】使用；你使用【探听】观看目标角色手牌时，可以展示其中的任意一张牌。",
	["jibao"] = "急报",
	[":jibao"] = "回合结束时，若你的手牌数与你本回合开始时的手牌数相等，则你可以弃置一张手牌。若如此做，则回合结束后，你进行一个额外的回合。",

	["#caijing"] = "奸相",
	["caijing"] = "蔡京",
	["jiashu"] = "家书",
	[":jiashu"] = "出牌阶段，你可以将一张手牌交给任一其他角色并你声明一种花色，若该角色交给你一张你所声明花色的手牌，则其摸一张牌，否则失去1点体力。每阶段限一次。",
	["duoquan"] = "夺权",
	[":duoquan"] = "限定技，当任一其他角色死亡时，若杀死该角色的不是你，则你可以获得该角色的所有牌和一项技能（主公技、限定技和觉醒技除外）。",
	["@quan"] = "孙权",
	["$jiashu1"] = "信中所言你可知晓？",
	["$jiashu2"] = "有为父在朝堂，尔等大可放心行事！",
	["$duoquan"] = "这朝事还不由我说了算！",
	["~caijing"] = "时至今日，方知百姓之恨！",

	["#lishishi"] = "绝色",
	["lishishi"] = "李师师",
	["coder:lishishi"] = "皇叔",
	["qinxin"] = "沁心",
	[":qinxin"] = "回合开始时，你可以声明一种花色并进行一次判定：若结果为你所声明的花色，则你回复1点体力，否则你获得该判定牌。",
	["yinjian"] = "引荐",
	[":yinjian"] = "出牌阶段，你可以指定两名势力不同的男性角色。你交给其中一名角色两张手牌，然后该角色交给另一名角色一张手牌。每阶段限一次。",
	["$qinxin1"] = "芳蓉丽质更妖娆，秋水精神瑞雪标。", --判定时播放
	["$qinxin2"] = "白玉生香花解语，千金良夜实难消。",
	["$yinjian1"] = "请诸位到此，少叙三杯。", --1和2给牌时随机播放
	["$yinjian2"] = "今晚，教你见天子一面。",
	["~lishishi"] = "陛下，妾身去矣！",

	["#yulan"] = "养娘",
	["yulan"] = "玉兰",
	["designer:yulan"] = "天尸娘",
	["qingdong"] = "情动",
	[":qingdong"] = "当一名其他男性角色回合外失去一张手牌时，你可以弃一张手牌并进行一次判定，若不为方块，将其手牌补至体力上限（最多为4张）。",
	["qingshang"] = "情殇",
	[":qingshang"] = "锁定技，杀死你的角色每回合开始阶段失去一点体力。",
	["@shang"] = "殇",
	["~yulan"] = "武都头……",
}
