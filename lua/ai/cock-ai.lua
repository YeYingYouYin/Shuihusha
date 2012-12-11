
-- fanwu
sgs.ai_skill_use["@@fanwu"] = function(self, prompt)
	if self.player:isKongcheng() then return "." end
	local card = self.player:getRandomHandCard()
	local damage = self.player:getTag("FanwuStruct"):toDamage()
	local dmg = damage.damage
	if damage.to:getArmor() and damage.to:getArmor():objectName() == "vine" and damage.nature == sgs.DamageStruct_Fire then dmg = dmg + 1 end
	if damage.to:getArmor() and damage.to:getArmor():objectName() == "silver_lion" then dmg = 1 end
	if damage.to:getRole() == "loyal" and self:isEnemy(damage.to) and damage.to:getHp() - dmg < 1
		and self.room:getLord() and self.room:getLord():getGeneral():isMale() then
		return "@FanwuCard=" .. card:getEffectiveId() .. "->" .. self.room:getLord():objectName()
	end
	if damage.to:getRole() == "rebel" and self:isEnemy(damage.from) and damage.to:getHp() - dmg < 1 then
		self:sort(self.friends, "handcard")
		for _, t in ipairs(self.friends) do
			if t:getGeneral():isMale() then
				return "@FanwuCard=" .. card:getEffectiveId() .. "->" .. t:objectName()
			end
		end
	end
	return "."
end

-- panxin
sgs.ai_skill_invoke["panxin"] = sgs.ai_skill_invoke["dujian"]

-- foyuan
function sgs.ai_slash_prohibit.foyuan(self)
	if self.player:getGeneral():isMale() and not self.player:hasEquip() then return true end
end

-- banzhuang
local banzhuang_skill={}
banzhuang_skill.name = "banzhuang"
table.insert(sgs.ai_skills, banzhuang_skill)
banzhuang_skill.getTurnUseCard = function(self,inclusive)
	local cards = self.player:getHandcards()
	cards = sgs.QList2Table(cards)
	for _,card in ipairs(cards)  do
		if card:getSuit() == sgs.Card_Heart or inclusive then
			local number = card:getNumberString()
			local card_id = card:getEffectiveId()
			local card_str = ("ex_nihilo:banzhuang[heart:%s]=%d"):format(number, card_id)
			local exnihilo = sgs.Card_Parse(card_str)
			assert(exnihilo)
			return exnihilo
		end
	end
end

-- zhuying
local zhuying_skill={}
zhuying_skill.name = "zhuying"
table.insert(sgs.ai_skills, zhuying_skill)
zhuying_skill.getTurnUseCard = function(self)
	if not self.player:isWounded() then return end
	local cards = self.player:getCards("h")
	cards=sgs.QList2Table(cards)
	local anal_card
	self:sortByUseValue(cards, true)
	for _,card in ipairs(cards)  do
		if card:inherits("Analeptic") then
			anal_card = card
			break
		end
	end
	if anal_card then
		local suit = anal_card:getSuitString()
		local number = anal_card:getNumberString()
		local card_id = anal_card:getEffectiveId()
		local card_str = ("peach:zhuying[%s:%s]=%d"):format(suit, number, card_id)
		local peach = sgs.Card_Parse(card_str)
		return peach
	end
end
sgs.ai_filterskill_filter["zhuying"] = function(card, card_place)
	local suit = card:getSuitString()
	local number = card:getNumberString()
	local card_id = card:getEffectiveId()
	if card:inherits("Analeptic") then return ("peach:zhuying[%s:%s]=%d"):format(suit, number, card_id) end
end

-- zishi
sgs.ai_skill_use["@@zishi"] = function(self, prompt)
	if self.player:isKongcheng() then return "." end
	local target = self.player:getTag("ZishiSource"):toPlayer()
	local cards = self.player:getHandcards()
	cards=sgs.QList2Table(cards)
	self:sortByUseValue(cards, true)
	for _,card in ipairs(cards) do
		if card:isBlack() and self:getUseValue(card) < 6 then
			if (self:isFriend(target) and target:getHandcardNum() < 3) or self:isEnemy(target) then
				return "@ZishiCard=" .. card:getEffectiveId() .. "->."
			end
		end
	end
	return "."
end
sgs.ai_skill_choice["zishi"] = function(self, choice)
	local source = self.player:getTag("ZishiSource"):toPlayer()
	if self:isFriend(source) then
		return "duo"
	else
		return "shao"
	end
end

-- zhangshi
local zhangshi_skill={}
zhangshi_skill.name="zhangshi"
table.insert(sgs.ai_skills,zhangshi_skill)
zhangshi_skill.getTurnUseCard=function(self)
	if self.player:hasUsed("ZhangshiCard") or not self:slashIsAvailable() then return end
	local card_str = "@ZhangshiCard=."
	local slash = sgs.Card_Parse(card_str)
	assert(slash)
	return slash
end
sgs.ai_skill_use_func["ZhangshiCard"] = sgs.ai_skill_use_func["GuibingCard"]
sgs.ai_skill_invoke["zhangshi"] = function(self, data)
	local cards = self.player:getHandcards()
	local slash = self:getCard("Slash")
	if slash then return false end
	return true
end
sgs.ai_skill_cardask["@zhangshi"] = function(self, data)
	local who = data:toPlayer()
	if not self:isFriend(who) then return "." end
	return self:getCardId("Slash") or "."
end

-- eyan
local eyan_skill={}
eyan_skill.name = "eyan"
table.insert(sgs.ai_skills, eyan_skill)
eyan_skill.getTurnUseCard = function(self)
	local jink = self:getCard("Jink")
	if self.player:hasUsed("EyanCard") or not jink then return end
	return sgs.Card_Parse("@EyanCard=.")
end
sgs.ai_skill_use_func["EyanCard"]=function(card,use,self)
	self:sort(self.enemies, "handcard")
	for _, enemy in ipairs(self.enemies) do
		if enemy:inMyAttackRange(self.player) and enemy:getGeneral():isMale() then
			use.card = card
			if use.to then use.to:append(enemy) end
			return
		end
	end
end

-- chiyuan
sgs.ai_skill_choice["chiyuan"] = function(self, choice)
	if self.player:isWounded() then
		return "qiao"
	else
		return "nu"
	end
end
sgs.ai_skill_cardask["@chiyuan"] = function(self, data)
	local rv = data:toRecover()
	if rv.card:inherits("SilverLion") then return "." end -- will crash
	local cards = self.player:getCards("he")
	cards=sgs.QList2Table(cards)
	self:sortByUseValue(cards, true)
	if self:isEnemy(rv.who) then
		return cards[1]:getEffectiveId()
	end
	return "."
end

-- huoshui
local huoshui_skill={}
huoshui_skill.name = "huoshui"
table.insert(sgs.ai_skills, huoshui_skill)
huoshui_skill.getTurnUseCard = function(self)
	local cards = self.player:getCards("h")
	cards=sgs.QList2Table(cards)
	local driver_card
	self:sortByUseValue(cards, true)
	for _,card in ipairs(cards)  do
		if card:inherits("Weapon") or card:inherits("Slash") then
			driver_card = card
			break
		end
	end
	if driver_card then
		local suit = driver_card:getSuitString()
		local number = driver_card:getNumberString()
		local card_id = driver_card:getEffectiveId()
		local card_str = ("drivolt:huoshui[%s:%s]=%d"):format(suit, number, card_id)
		local driver = sgs.Card_Parse(card_str)
		return driver
	end
end
sgs.ai_filterskill_filter["huoshui"] = function(card, card_place)
	local suit = card:getSuitString()
	local number = card:getNumberString()
	local card_id = card:getEffectiveId()
	if card:inherits("Weapon") or card:inherits("Slash") then return ("drivolt:huoshui[%s:%s]=%d"):format(suit, number, card_id) end
end

-- zhiyu
sgs.ai_skill_invoke["zhiyu"] = function(self, data)
	local player = data:toPlayer()
	self.zhiyusource = player
	return not self.player:isKongcheng()
end
sgs.ai_skill_askforag["zhiyu"] = function(self, card_ids)
	local cards = {}
	for _, card_id in ipairs(card_ids)  do
		table.insert(cards, sgs.Sanguosha:getCard(card_id))
	end
	if self:isFriend(self.zhiyusource) then
		self:sortByUseValue(cards, true)
	else
		self:sortByUseValue(cards)
	end
	return cards[1]:getEffectiveId()
end
sgs.ai_cardshow["zhiyu"] = function(self, requestor)
	local cards = self.player:getHandcards()
	cards = sgs.QList2Table(cards)
	if self:isFriend(requestor) then
		self:sortByUseValue(cards)
	else
		self:sortByUseValue(cards, true)
	end
	return cards[1]
end

-- baoen
sgs.ai_skill_cardask["@baoen"] = function(self, data)
	local rev = data:toRecover()
	local cards = sgs.QList2Table(self.player:getHandcards())
	self:sortByUseValue(cards, true)
	if self:isEnemy(rev.who) or not cards[1] then return "." end
	return cards[1]:getEffectiveId() or "."
end
