-- AI for dragon package

-- liutang
-- xiashu
sgs.ai_card_intention.XiashuCard = -80

xiashu_skill={}
xiashu_skill.name = "xiashu"
table.insert(sgs.ai_skills, xiashu_skill)
xiashu_skill.getTurnUseCard = function(self)
	if not self.player:hasUsed("XiashuCard") and not self.player:isNude() then
		local cards = self.player:getCards("he")
		cards=sgs.QList2Table(cards)
		self:sortByUseValue(cards, true)
		return sgs.Card_Parse("@XiashuCard=" .. cards[1]:getEffectiveId())
	end
end
sgs.ai_skill_use_func["XiashuCard"]=function(card,use,self)
	self:sort(self.friends)
	if use.to then
		self:speak("xiashu")
		use.to:append(self.friends[1])
	end
	use.card=card
end

-- hantao
sgs.hantao_suit_value = 
{
	spade = 5,
}

-- taolue
local taolue_skill={}
taolue_skill.name = "taolue"
table.insert(sgs.ai_skills, taolue_skill)
taolue_skill.getTurnUseCard = function(self)
	if not self.player:hasUsed("TaolueCard") and not self.player:isKongcheng() then
		local max_card
		local cards = self.player:getHandcards()
		cards = sgs.QList2Table(cards)
		for _, card in ipairs(cards) do
			if card:getSuit() == sgs.Card_Spade then
				max_card = card
				break
			end
		end
		if not max_card then max_card = self:getMaxCard() end
		return sgs.Card_Parse("@TaolueCard=" .. max_card:getEffectiveId())
	end
end
sgs.ai_skill_use_func["TaolueCard"]=function(card,use,self)
	self:sort(self.enemies, "handcard")
	for _, enemy in ipairs(self.enemies) do
		if not enemy:isKongcheng() and not enemy:getEquips():isEmpty() then
			use.card=card
			if use.to then
				self:speak("taolue")
				use.to:append(enemy)
			end
			return
		end
	end
end
sgs.ai_skill_playerchosen["taolue"] = sgs.ai_skill_playerchosen["lihun"]

-- shibao
sgs.shibao_keep_value =
{
	Slash = 4,
}

-- xiaozhan
sgs.ai_skill_cardask["@xiaozhan"] = function(self, data)
	local use = data:toCardUse()
	local to = sgs.QList2Table(use.to)
	for _, enemy in ipairs(to) do
		if enemy:getHandcardNum() > 0 and enemy:getHandcardNum() < 2 then
			local cards = sgs.QList2Table(self.player:getCards("he"))
			self:sortByUseValue(cards, true)
			self:speak("xiaozhan")
			return cards[1]:getEffectiveId()
		end
	end
	return "."
end

-- ruanxiaowu
-- anxi
sgs.ai_card_intention.AnxiCard = 100

sgs.ai_skill_use["@@anxi"] = function(self, prompt)
	local cards = self.player:getHandcards()
	cards = sgs.QList2Table(cards)
	self:sortByUseValue(cards, true)
	local card
	for _, c in ipairs(cards) do
		if c:isRed() then
			if self:getUseValue(c) < 6 then
				card = c
			elseif self:getCardsNum("Peach") > 2 then
				card = c
			elseif not self.player:isWounded() and self.player:getHandcardNum() > 2 then
				card = c
			end
			if card then break end
		end
	end
	if not card then return "." end
	self:sort(self.enemies, "hp2")
	for _, enemy in ipairs(self.enemies) do
		if enemy:getHp() >= self.player:getHp() then
			self:speak("anxi")
			return "@AnxiCard=" .. card:getEffectiveId() .. "->" .. enemy:objectName()
		end
	end
	return "."
end

-- shuilao
shuilao_skill={}
shuilao_skill.name="shuilao"
table.insert(sgs.ai_skills,shuilao_skill)
shuilao_skill.getTurnUseCard=function(self,inclusive)
	local cards = self.player:getCards("he")
	cards=sgs.QList2Table(cards)
	local card
	self:sortByUseValue(cards, true)
	for _,acard in ipairs(cards) do
		if acard:isKindOf("EquipCard") and
		((self:getUseValue(acard) < sgs.ai_use_value.Indulgence) or inclusive) then
			card = acard
			break
		end
	end

	if not card then return nil end
	local number = card:getNumberString()
	local card_id = card:getEffectiveId()
	local card_suit = card:getSuitString()
	local card_str = ("indulgence:shuilao[%s:%s]=%d"):format(card_suit, number, card_id)
	local indulgence = sgs.Card_Parse(card_str)
	assert(indulgence)
	return indulgence
end

-- zhengtianshou
sgs.zhengtianshou_keep_value =
{
	TrickCard = 3
}

-- wugou
local wugou_skill={}
wugou_skill.name = "wugou"
table.insert(sgs.ai_skills,wugou_skill)
wugou_skill.getTurnUseCard = function(self)
	local first_found, second_found = false, false
	local first_card, second_card
	if self.player:getHandcardNum() >= 2 then
		local cards = self.player:getHandcards()
		local same_suit=false
		cards = sgs.QList2Table(cards)
		for _, fcard in ipairs(cards) do
			if not fcard:isKindOf("Peach") and fcard:isKindOf("BasicCard") then
				first_card = fcard
				first_found = true
				for _, scard in ipairs(cards) do
					if first_card ~= scard and scard:isKindOf("BasicCard") and
						(scard:isRed() and first_card:isRed()) and 
						not scard:isKindOf("Peach") then
						second_card = scard
						second_found = true
						break
					end
				end
				if second_card then break end
			end
		end
	end
	if first_found and second_found then
		local wugou_card = {}
		local first_suit, first_number, first_id = first_card:getSuitString(), first_card:getNumberString(), first_card:getId()
		local second_suit, second_number, second_id = second_card:getSuitString(), second_card:getNumberString(), second_card:getId()
		local card_str = ("assassinate:wugou[%s:%s]=%d+%d"):format(first_suit, first_number, first_id, second_id)
		local assassinate = sgs.Card_Parse(card_str)
		assert(assassinate)
		self:speak("wugou")
		return assassinate
	end
end

-- qiaojiang
local qiaojiang_skill={}
qiaojiang_skill.name = "qiaojiang"
table.insert(sgs.ai_skills, qiaojiang_skill)
qiaojiang_skill.getTurnUseCard = function(self, inclusive)
	local cards = self.player:getCards("h")
	cards=sgs.QList2Table(cards)
	local card
	self:sortByUseValue(cards, true)
	for _, car in ipairs(cards)  do
		if car:isBlack() and car:isKindOf("TrickCard") and
			((self:getUseValue(car) < sgs.ai_use_value.Slash) or inclusive) then
			card = car
			break
		end
	end
	if card then
		local suit = card:getSuitString()
		local number = card:getNumberString()
		local card_id = card:getEffectiveId()
		local card_str = ("slash:qiaojiang[%s:%s]=%d"):format(suit, number, card_id)
		local slash = sgs.Card_Parse(card_str)
		assert(slash)
		return slash
	end
end
sgs.ai_view_as["qiaojiang"] = function(card, player, card_place)
	local suit = card:getSuitString()
	local number = card:getNumberString()
	local card_id = card:getEffectiveId()

	if card:isBlack() and card:isKindOf("TrickCard") then
		return ("slash:qiaojiang[%s:%s]=%d"):format(suit, number, card_id)
	elseif card:isRed() and card:isKindOf("TrickCard") then
		return ("jink:qiaojiang[%s:%s]=%d"):format(suit, number, card_id)
	end
end

-- gaoyanei
-- jiandiao
sgs.ai_skill_choice["jiandiao"] = function(self, choice, data)
	self:speak("jiandiao")
	if choice == "diao+nil" then return "diao" end
	local source = data:toDamage().from
	if self:isFriend(source) then
		return "diao"
	else
		local delta = math.abs(self.player:getHp() - source:getHp())
		if delta > 2 and math.random(0,2) ~= 1 then
			return "diao"
		else
			return "jian"
		end
	end
end

-- shantinggui
sgs.shantinggui_keep_value =
{
	BasicCard = 4,
}

-- shuizhen
sgs.ai_skill_cardask["@shuizhen1"] = function(self, data)
	local damage = data:toDamage()
	if not self:isFriend(damage.to) or damage.damage < 1 then return "." end
	local cards = sgs.QList2Table(self.player:getHandcards())
	self:sortByUseValue(cards, false)
	self:speak("shuizhenf")
	return cards[1]:getEffectiveId()
end
sgs.ai_skill_cardask["@shuizhen2"] = function(self, data)
	local damage = data:toDamage()
--	local players = {}
	local fri = #(self:getChainedEnemies())
	local eni = #(self:getChainedFriends())
	local fri, eni = 0, 0
	for _, t in sgs.qlist(self.room:getOtherPlayers(self.player)) do
		if t ~= damage.to and self:damageIsEffective(t, sgs.DamageStruct_Thunder, damage.from) then
			if self:isEnemy(t) then
				eni = eni + 1
			else
				fri = fri + 1
			end
--			table.insert(players, t)
		end
	end
	if fri > eni then return "." end
	local cards = sgs.QList2Table(self.player:getHandcards())
	self:sortByUseValue(cards, false)
	for _, fcard in ipairs(cards) do
		if fcard:isKindOf("BasicCard") and
			not fcard:isKindOf("Peach") and not fcard:isKindOf("Analeptic") then
			self:speak("shuizhent")
			return fcard:getEffectiveId()
		end
	end
	return "."
end

-- yanmo
sgs.ai_card_intention.YanmoCard = 80
sgs.ai_skill_use["@@yanmo"] = function(self, prompt)
	local ene2, ene1 = {}, {}
	for _, enemy in ipairs(self.enemies) do
		if (enemy:getWeapon() and enemy:getArmor()) or
			(enemy:getOffensiveHorse() and enemy:getDefensiveHorse()) then
			table.insert(ene2, enemy)
		end
	end
	if #ene2 > 0 then
		self:sort(ene2)
		return "@YanmoCard=.->" .. ene2[1]:objectName()
	end
	for _, enemy in ipairs(self.enemies) do
		if enemy:hasEquip() then
			table.insert(ene1, enemy)
		end
	end
	if #ene1 > 0 then
		self:sort(ene1)
		return "@YanmoCard=.->" .. ene1[1]:objectName()
	end
	return "."
end
sgs.ai_skill_choice["yanmo"] = function(self, choice, data)
	local target = data:toPlayer()
	local yan, mo = 0, 0
	if target:getWeapon() then yan = yan + 1 end
	if target:getArmor() then yan = yan + 1 end
    if target:getOffensiveHorse() then mo = mo + 1 end
    if target:getDefensiveHorse() then mo = mo + 1 end
	if yan >= mo then return "yan" else return "mo" end
end

-- lizhu
-- chuqiao
sgs.ai_skill_invoke["chuqiao"] = true

-- jianwu
sgs.ai_skill_invoke["jianwu"] = function(self, data)
	local from = data:toPlayer()
	self.lizhu = self.player
	return self:isEnemy(from)
end
sgs.ai_skill_cardask["@jianwu-slash"] = function(self, data)
	local target = data:toPlayer()
	local card = self:getCard("Slash")
	if self:isEnemy(target) and card then
		if not self:slashProhibit(target, card, self.lizhu) then
			self:speak("jianwu", self.lizhu)
			return card:getEffectiveId()
		end
	end
	return "."
end

-- yangchun
-- shexin
sgs.ai_card_intention.ShexinCard = 100

local shexin_skill={}
shexin_skill.name = "shexin"
table.insert(sgs.ai_skills, shexin_skill)
shexin_skill.getTurnUseCard = function(self)
	if not self.player:hasUsed("ShexinCard") and not self.player:isNude() then
		self:sort(self.enemies, "handcard2")
		if #self.enemies == 0 or self.enemies[1]:getHandcardNum() <= 3 then return end
		local cards = self.player:getCards("he")
		cards = sgs.QList2Table(cards)
		self:sortByUseValue(cards, true)
		for _, card in ipairs(cards) do
			if card:isNDTrick() or card:isKindOf("EquipCard") then
				self:speak("shexin")
				return sgs.Card_Parse("@ShexinCard=" .. card:getEffectiveId())
			end
		end
	end
end
sgs.ai_skill_use_func["ShexinCard"] = function(card,use,self)
	self:sort(self.enemies, "handcard2")
	if use.to and #self.enemies > 0 then
		use.to:append(self.enemies[1])
	end
	use.card=card
end

-- qiongyaonayan
sgs.qiongyaonayan_suit_value = 
{
	spade = 4,
	club = 4,
}

-- jiaozhen
sgs.ai_skill_invoke["jiaozhen"] = function(self, data)
	local hole = self.player:getHandcardNum()
	if hole < 2 then return false end
	local cards = sgs.QList2Table(self.player:getCards("h"))
	local black = 0
	for _, card in ipairs(cards) do
		if card:isBlack() then
			black = black + 1
		end
	end
	return black * 2 >= hole
end
jiaozhen_skill={}
jiaozhen_skill.name = "jiaozhen"
table.insert(sgs.ai_skills, jiaozhen_skill)
jiaozhen_skill.getTurnUseCard = function(self)
	if not self.player:hasFlag("Bark") then return end
	local cards = self.player:getCards("h")
	cards=sgs.QList2Table(cards)
	local card
	self:sortByUseValue(cards,true)
	for _,acard in ipairs(cards)  do
		if acard:isBlack() then
			card = acard
			break
		end
	end
	if not card then return nil end
	local suit, number, card_id = card:getSuitString(), card:getNumberString(), card:getEffectiveId()
	local card_str = ("duel:jiaozhen[%s:%s]=%d"):format(suit, number, card_id)
	local duel = sgs.Card_Parse(card_str)
	assert(duel)
	self:speak("jiaozhen")
	return duel
end

-- suochao
-- chongfeng
sgs.ai_skill_invoke["chongfeng"] = function(self)
	self:speak("chongfeng")
	return true
end

-- wangpo
sgs.wangpo_suit_value = 
{
	club = 5,
}

-- qianxian
local qianxian_skill={}
qianxian_skill.name = "qianxian"
table.insert(sgs.ai_skills, qianxian_skill)
qianxian_skill.getTurnUseCard = function(self)
	if self.player:hasUsed("QianxianCard") then return end
	local cards = self.player:getCards("h")
	cards = sgs.QList2Table(cards)
	for _, acard in ipairs(cards) do
		if acard:isNDTrick() and acard:isBlack() then
			return sgs.Card_Parse("@QianxianCard=" .. acard:getEffectiveId())
		end
	end
end
sgs.ai_skill_use_func["QianxianCard"] = function(card,use,self)
	self:sort(self.enemies, "handcard")
	local first, second
	for _, tmp in ipairs(self.enemies) do
		if not tmp:isChained() or tmp:faceUp() then
			if not first then
				first = tmp
			elseif tmp:getMaxHP() ~= first:getMaxHP() then
				second = tmp
			end
			if first and second then break end
		end
	end
	if not first then
		for _, tmp in ipairs(self.friends_noself) do
			if tmp:getHandcardNum() > 2 and (not tmp:faceUp() or tmp:isChained()) then
				first = tmp
			elseif tmp:getMaxHP() ~= first:getMaxHP() then
				second = tmp
			end
			if first and second then break end
		end
	elseif not second then
		for _, tmp in ipairs(self.friends_noself) do
			if tmp:getHandcardNum() > 2 and (not tmp:faceUp() or tmp:isChained()) then
				second = tmp
			end
			if first and second then break end
		end
	end
	if first and second then
		use.card = card
		if use.to then
			self:speak("qianxian")
			use.to:append(first)
			use.to:append(second)
		end
	end
end

-- meicha
meicha_skill={}
meicha_skill.name = "meicha"
table.insert(sgs.ai_skills, meicha_skill)
meicha_skill.getTurnUseCard = function(self)
	local cards = self.player:getCards("h")
	cards=sgs.QList2Table(cards)
	local card
	self:sortByUseValue(cards,true)
	for _,acard in ipairs(cards)  do
		if (acard:getSuit() == sgs.Card_Club) then
			card = acard
			break
		end
	end
	if not card then return nil end
	local number = card:getNumberString()
	local card_id = card:getEffectiveId()
	local card_str = ("analeptic:meicha[club:%s]=%d"):format(number, card_id)
	local analeptic = sgs.Card_Parse(card_str)
	assert(analeptic)
	return analeptic
end
sgs.ai_view_as["meicha"] = function(card, player, card_place)
	local suit = card:getSuitString()
	local number = card:getNumberString()
	local card_id = card:getEffectiveId()

	if card_place ~= sgs.Player_Equip and card:getSuit() == sgs.Card_Club then
		return ("analeptic:meicha[%s:%s]=%d"):format(suit, number, card_id)
	end
end

