-- AI for ox package

-- gaolian
-- guibing
sgs.ai_card_intention.GuibingCard = sgs.ai_card_intention.Slash

sgs.ai_skill_invoke["guibing"] = sgs.ai_skill_invoke["shalu"]
guibing_skill = {}
guibing_skill.name = "guibing"
table.insert(sgs.ai_skills, guibing_skill)
guibing_skill.getTurnUseCard = function(self)
	if self.player:hasFlag("Guibing") or not self:slashIsAvailable() then return end
	local card_str = "@GuibingCard=."
	local slash = sgs.Card_Parse(card_str)
	assert(slash)
	return slash
end
sgs.ai_skill_use_func["GuibingCard"] = function(card,use,self)
	self:sort(self.enemies, "defense")
	local target_count=0
	local slash = sgs.Sanguosha:cloneCard("slash", sgs.Card_NoSuit, 0)
	for _, enemy in ipairs(self.enemies) do
		if ((self.player:canSlash(enemy, not no_distance)) or
			(use.isDummy and (self.player:distanceTo(enemy)<=self.predictedRange))) and
			self:objectiveLevel(enemy)>3 and
			self:slashIsEffective(slash, enemy) and
			not self:slashProhibit(slash, enemy) then
			local cheat_card = sgs.Sanguosha:getCard(self.room:getDrawPile():first())
			if cheat_card and cheat_card:getSuit() == sgs.Card_Heart and not self.player:hasUsed("HeiwuCard") and not self.player:isKongcheng() then
				local cards = sgs.QList2Table(self.player:getCards("h"))
				self:sortByUseValue(cards, true)
				for _, car in ipairs(cards) do
					if car:getSuit() ~= sgs.Card_Heart then
						use.card = sgs.Card_Parse("@HeiwuCard=" .. car:getId())
						return
					end
				end
			end
			use.card=card
			if use.to then
				use.to:append(enemy)
			end
			target_count=target_count+1
			if self.slash_targets<=target_count then return end
		end
	end
end
--function sgs.ai_cardneed.guibing(to, card, self)
--	return card:isBlack()
--end

-- tongguan
-- zhengfa
sgs.ai_card_intention.ZhengfaCard = function(card, from, tos)
	if from:hasFlag("Zhengfa") then
		sgs.updateIntentions(from, tos, 100)
	else
		sgs.updateIntentions(from, tos, 10)
	end
end

sgs.ai_skill_use["@@zhengfa"] = function(self, prompt)
	if self.player:hasFlag("Zhengfa") then
		local enemies = {}
		local i = 0
		local king = self.room:getKingdoms()
		local slash = sgs.Sanguosha:cloneCard("slash", sgs.Card_NoSuit, 0)
		self:sort(self.enemies, "handcard")
		for _, enemy in ipairs(self.enemies) do
			if not self:slashProhibit(slash, enemy) then
				table.insert(enemies, enemy:objectName())
				i = i + 1
			end
			if i >= king then break end
		end
		if king >= #enemies and #enemies > 0 then
			return "@ZhengfaCard=.->" .. table.concat(enemies, "+")
		else
			return "."
		end
	end
	local max_card = self:getMaxCard()
	self:sort(self.friends_noself, "handcard")
	for i = #self.friends_noself, 1, -1 do
		if not self.friends_noself[i]:isKongcheng() and self.player:getKingdom() ~= self.friends_noself[i]:getKingdom() then
			if max_card and max_card:getNumber() > 7 then
				return "@ZhengfaCard=" .. max_card:getEffectiveId() .. "->" .. self.friends_noself[i]:objectName()
			end
		end
	end
	return "."
end

-- huyanzhuo
-- lianma
lianma_skill={}
lianma_skill.name = "lianma"
table.insert(sgs.ai_skills, lianma_skill)
lianma_skill.getTurnUseCard = function(self)
	if self.player:hasUsed("LianmaCard") then return end
	return sgs.Card_Parse("@LianmaCard=.")
end
sgs.ai_skill_use_func["LianmaCard"] = function(card, use, self)
	use.card = card
end
sgs.ai_skill_choice["lianma"] = function(self, choice)
	local jie_f = 0
	local lian_e = 0
	for _, enemy in ipairs(self.enemies) do
		if enemy:hasEquip("Horse", true) and not enemy:isChained() then
			lian_e = lian_e + 1
		end
	end
	for _, friend in ipairs(self.friends) do
		if not friend:hasEquip("Horse", true) and friend:isChained()then
			jie_f = jie_f + 1
		end
	end

	if lian_e >= jie_f then
		return "lian"
	else
		return "ma"
	end
end

-- dongchaoxueba
sgs.dongchaoxueba_suit_value =
{
	spade = 3.8,
	club = 3.8
}
sgs.dongchaoxueba_keep_value =
{
	BasicCard = 3.8
}

-- sheru
sgs.ai_card_intention.SheruCard = function(card, from, tos)
	if tos[1]:isWounded() and tos[1]:getHp() > 1 then
		sgs.updateIntentions(from, tos, -40)
	elseif tos[1]:getLostHp() == 1 then
		sgs.updateIntentions(from, tos, 40)
	else
		sgs.updateIntentions(from, tos, math.random(-10, 10))
	end
end

sheru_skill={}
sheru_skill.name = "sheru"
table.insert(sgs.ai_skills, sheru_skill)
sheru_skill.getTurnUseCard = function(self)
	if self.player:hasUsed("SheruCard") then return end
	local cards = self.player:getCards("h")
	cards=sgs.QList2Table(cards)
	self:sortByUseValue(cards, true)
	for _, card in ipairs(cards) do
		if card:isBlack() and card:inherits("BasicCard") then
			return sgs.Card_Parse("@SheruCard=" .. card:getEffectiveId())
		end
	end
end
sgs.ai_skill_use_func["SheruCard"] = function(card, use, self)
	self:sort(self.friends_noself)
	for _, friend in ipairs(self.friends_noself) do
		if friend:isWounded() and friend:getHp() > 1 then
			use.card = card
			if use.to then
				self.sherutarget = friend
				use.to:append(friend)
			end
			return
		end
	end
	for _, enemy in ipairs(self.enemies) do
		if enemy:getLostHp() == 1 then
			use.card = card
			if use.to then
				self.sherutarget = enemy
				use.to:append(enemy)
			end
			return
		end
	end
end
sgs.ai_skill_choice["sheru"] = function(self, choice)
	if self:isEnemy(self.sherutarget) then
		return "she"
	elseif self.sherutarget:getCardCount(true) > 2 and self.sherutarget:getLostHp() <= 2 then
		return "ru"
	else
		return "she"
	end
end

-- pangwanchun
-- lianzhu
local lianzhu_skill = {}
lianzhu_skill.name = "lianzhu"
table.insert(sgs.ai_skills, lianzhu_skill)
lianzhu_skill.getTurnUseCard = function(self)
	if not self.player:faceUp() or self.player:hasUsed("LianzhuCard") or self:isWeak() then return end
	if #self.enemies > #self.friends_noself then
		return sgs.Card_Parse("@LianzhuCard=.")
	end
end
sgs.ai_skill_use_func["LianzhuCard"] = function(card,use,self)
	if not use.isDummy then self:speak("lianzhu") end
	use.card=card
end

-- huangxin
-- tongxia
sgs.ai_skill_invoke["tongxia"] = true
sgs.ai_skill_askforag["tongxia"] = function(self, card_ids)
	return card_ids[1]
end
sgs.ai_skill_playerchosen["tongxia"] = function(self, targets)
	local card = self.player:getTag("TongxiaCard"):toCard()
	self:sort(self.enemies, "hp")
	for _, enemy in ipairs(self.enemies) do
		if card:inherits("GaleShell") or card:inherits("Shit") then
			return enemy
		end
	end
	self:sort(self.enemies, "defense")
	for _, friend in ipairs(self.friends) do
		if card:inherits("EquipCard") then
			if (card:inherits("Weapon") and not friend:getWeapon()) or
				(card:inherits("Armor") and (not friend:getArmor() or self:isEquip("GaleShell", friend))) or
				(card:inherits("DefensiveHorse") and not friend:getDefensiveHorse()) or
				(card:inherits("OffensiveHorse") and not friend:getOffensiveHorse()) then
				return friend
			end
		else
			return self.player
		end
	end
	return self.friends_noself[1]
end

-- luozhenren
-- butian
sgs.ai_skill_cardask["@butian-card"] = function(self, data)
	local judge = data:toJudge()

	if self:needRetrial(judge) and not self.player:isKongcheng() then
		local cards = sgs.QList2Table(self.player:getHandcards())
		self:sortByUseValue(cards, true)
		self.butianjudge = judge
	--	return "@ButianCard[" .. cards[1]:getSuitString() .. ":" .. cards[1]:getNumberString() .. "]=" .. cards[1]:getEffectiveId()
		return "@ButianCard=" .. cards[1]:getEffectiveId()
	end
	return "."
end
sgs.ai_skill_askforag["butian"] = function(self, card_ids)
	local judge = self.butianjudge
	local cards = {}
	local card_id
	if judge and self:needRetrial(judge) then
		for _, card_id in ipairs(card_ids) do
			local card = sgs.Sanguosha:getCard(card_id)
			table.insert(cards, card)
		end
		card_id = self:getRetrialCardId(cards, judge)
		if card_id ~= -1 then
			return card_id
		end
	end
	return card_ids[1]
end

-- huaxian
sgs.ai_skill_invoke["huaxian"] = true

-- lili
sgs.lili_suit_value =
{
	spade = 3.7,
	club = 3.7
}

-- moucai
sgs.ai_skill_invoke["moucai"] = sgs.ai_skill_invoke["qiongtu"]

-- duoming
sgs.ai_skill_use["@@duoming"] = function(self, prompt)
	local target = self.room:getCurrent()
	if self:isFriend(target) then return "." end
	if self.player:getHandcardNum() > 2 then
		local cards = self.player:getHandcards()
		cards = sgs.QList2Table(cards)
		self:sortByUseValue(cards, true)
		local card_ids = {}
		for _, card in ipairs(cards) do
			if #card_ids < 2 and card:isBlack() then
				table.insert(card_ids, card:getEffectiveId())
			end
		end
		if #card_ids == 2 then
			return "@DuomingCard=" .. table.concat(card_ids, "+") .. "->."
		end
	end
	return "."
end

-- shijin
sgs.shijin_keep_value =
{
	EquipCard = 5
}

-- wubang
sgs.ai_skill_invoke["wubang"] = true

-- xiagu
sgs.ai_skill_cardask["@xiagu"] = function(self, data)
	local damage = data:toDamage()
	if self:isFriend(damage.to) then
		if damage.to:hasSkill("fushang") and
			damage.to:getMaxHP() > 3 and damage.damage < 2 then return "." end
		local allcards = self.player:getCards("he")
		for _, card in sgs.qlist(allcards) do
			if card:inherits("EquipCard") then
				return card:getEffectiveId()
			end
		end
	end
	return "."
end
function sgs.ai_cardneed.xiagu(to, card, self)
	return card:inherits("EquipCard")
end

-- lijun
sgs.lijun_keep_value =
{
	TrickCard = 5
}

-- nizhuan
sgs.ai_skill_invoke["nizhuan"] = true

-- dingce
dingce_skill={}
dingce_skill.name = "dingce"
table.insert(sgs.ai_skills, dingce_skill)
dingce_skill.getTurnUseCard = function(self)
	if self.player:hasUsed("DingceCard") then return end
	local trick = self:getCard("TrickCard")
	if not trick or #self.friends == 1 then return end
	return sgs.Card_Parse("@DingceCard=" .. trick:getEffectiveId())
end
sgs.ai_skill_use_func["DingceCard"] = function(card, use, self)
	local target = self.enemies[1]
	if self.player:isWounded() then
		local erniang = self.room:findPlayerBySkillName("heidian")
		if erniang then
			self:sort(self.enemies, "handcard")
			target = self.enemies[1]
		else
			local players = sgs.QList2Table(self.room:getOtherPlayers(self.player))
			self:sort(players, "handcard")
			target = players[1]
		end
		use.card = card
		if use.to then use.to:append(target) end
		return
	else
		use.card = card
		if use.to and #self.friends_noself ~= 0 then use.to:append(self.friends_noself[1]) end
	end
end
sgs.ai_skill_invoke["dingce"] = function(self, data)
	return self.player:isWounded()
end

-- xiezhen
sgs.xiezhen_suit_value =
{
	heart = 3.5,
	diamond = 3.0
}

-- xunlie
sgs.ai_card_intention.XunlieCard = 80

sgs.ai_skill_use["@@xunlie"] = function(self, prompt)
	if self.player:getEquips():length() > 1 and #self.enemies > 1 then
		local enemies = {}
		local i = 0
		self:sort(self.enemies, "handcard")
		for _, enemy in ipairs(self.enemies) do
			if not enemy:isKongcheng() then
				table.insert(enemies, enemy:objectName())
				i = i + 1
			end
			if i >= self.player:getEquips():length() then break end
		end
		if #enemies > 0 then
			return "@XunlieCard=.->" .. table.concat(enemies, "+")
		end
	end
	local cards = self.player:getCards("h")
	cards = sgs.QList2Table(cards)
	self:sortByUseValue(cards, true)
	for _, acard in ipairs(cards) do
		if acard:isRed() then
			self:sort(self.enemies, "handcard")
			for _, enemy in ipairs(self.enemies) do
				if enemy:getHandcardNum() > 1 then
					return "@XunlieCard=" .. acard:getEffectiveId() .. "->" .. enemy:objectName()
				end
			end
		end
	end
	return "."
end

-- linniangzi
-- shouwang
sgs.ai_card_intention.ShouwangCard = -60

shouwang_skill={}
shouwang_skill.name = "shouwang"
table.insert(sgs.ai_skills, shouwang_skill)
shouwang_skill.getTurnUseCard = function(self)
	if self.player:hasUsed("ShouwangCard") then return end
	local slash = self:getCard("Slash")
	if not slash then return end
	return sgs.Card_Parse("@ShouwangCard=" .. slash:getEffectiveId())
end
sgs.ai_skill_use_func["ShouwangCard"] = function(card, use, self)
	self:sort(self.friends,"threat")
	for _, friend in ipairs(self.friends) do
		if friend:getGeneral():isMale() then
			use.card = card
			if use.to then use.to:append(friend) end
			return
		end
	end
end
sgs.ai_skill_choice["shouwang"] = function(self, choice, data)
	local effect = data:toCardEffect()
	if self:isEnemy(effect.to) then
		return "zi"
	elseif effect.from:getHandcardNum() > effect.to:getHandcardNum() then
		return "tian"
	else
		return "zi"
	end
end

-- zhongzhen
sgs.ai_skill_invoke["zhongzhen"] = function(self, data)
	local damage = data:toDamage()
	if damage.damage < 1 then return false end

	local max_card = self:getMaxCard()
	local max_care = self:getMaxCard(damage.from)
	if max_card and max_care and max_card:getNumber() > max_care:getNumber() then
		if self:isWeak() then
			return true
		else
			return self:isEnemy(damage.from)
		end
	else
		return false
	end
end
sgs.ai_skill_pindian["zhongzhen"] = function(minusecard, self, requestor, maxcard)
	local cards = sgs.QList2Table(self.player:getHandcards())
	local compare_func = function(a, b)
		return a:getNumber() > b:getNumber()
	end
	table.sort(cards, compare_func)
	for _, card in ipairs(cards) do
		if not card:inherits("Analeptic") and not card:inherits("Peach") then
			return card
		end
	end
end

