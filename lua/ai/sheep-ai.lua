-- AI for sheep package

-- tuzai&longjiao
sgs.ai_skill_invoke["tuzai"] = true
sgs.ai_skill_invoke["longjiao"] = true

-- zaochuan
local zaochuan_skill = {}
zaochuan_skill.name = "zaochuan"
table.insert(sgs.ai_skills, zaochuan_skill)
zaochuan_skill.getTurnUseCard = function(self)
	local cards = self.player:getCards("h")
	cards=sgs.QList2Table(cards)
	local card
	self:sortByUseValue(cards,true)
	for _,acard in ipairs(cards)  do
		if acard:getSuit() == sgs.Card_Club then
			card = acard
			break
		end
	end
	if not card then return nil end
	local number = card:getNumberString()
	local card_id = card:getEffectiveId()
	local card_str = ("iron_chain:zaochuan[club:%s]=%d"):format(number, card_id)
	local skillcard = sgs.Card_Parse(card_str)
	assert(skillcard)
	return skillcard
end

-- nusha
nusha_skill={}
nusha_skill.name = "nusha"
table.insert(sgs.ai_skills, nusha_skill)
nusha_skill.getTurnUseCard = function(self)
	if self.player:hasUsed("NushaCard") then return end
	local enum = 0
	for _, enemy in ipairs(self.enemies) do
		if enemy:getHandcardNum() > enum then
			enum = enemy:getHandcardNum()
		end
	end
	local fnum = 0
	for _, friend in ipairs(self.friends_noself) do
		if friend:getHandcardNum() > fnum then
			fnum = friend:getHandcardNum()
		end
	end
	if enum >= fnum then
		local slash = self:getCardId("Slash")
		if slash then
			return sgs.Card_Parse("@NushaCard=" .. slash)
		end
	end
	return
end
sgs.ai_skill_use_func["NushaCard"] = function(card, use, self)
	local enum = self.enemies[1]
	for _, enemy in ipairs(self.enemies) do
		if enemy:getHandcardNum() > enum:getHandcardNum() then
			enum = enemy
		end
	end
	use.card = card
	if use.to then
		use.to:append(enum)
	end
end

-- kongying
sgs.ai_skill_invoke["kongying"] = true
sgs.ai_skill_playerchosen["kongying"] = function(self, targets)
	self:sort(self.enemies, "hp")
	return self.enemies[1]
end
function sgs.ai_cardneed.kongying(to, card, self)
	return card:inherits("Jink")
end

-- feiqiang
feiqiang_skill={}
feiqiang_skill.name = "feiqiang"
table.insert(sgs.ai_skills, feiqiang_skill)
feiqiang_skill.getTurnUseCard = function(self)
	if not self.player:hasUsed("FeiqiangCard") then
		local cards = self.player:getCards("he")
		cards = sgs.QList2Table(cards)
		for _, acard in ipairs(cards)  do
			if acard:inherits("Weapon") then
				return sgs.Card_Parse("@FeiqiangCard=" .. acard:getEffectiveId())
			end
		end
	end
	return
end
sgs.ai_skill_use_func["FeiqiangCard"] = function(card, use, self)
	self:sort(self.enemies, "defense")
	use.card = card
	if use.to then
		use.to:append(self.enemies[1])
	end
end
sgs.ai_skill_choice["feiqiang"] = function(self, choices)
	return "gong"
end

-- citan
sgs.ai_skill_invoke["citan"] = sgs.ai_skill_invoke["lihun"]
sgs.ai_skill_askforag["citan"] = function(self, card_ids)
	local cards = {}
	for _, card_id in ipairs(card_ids) do
		local card = sgs.Sanguosha:getCard(card_id)
		table.insert(cards, card)
	end
	self:sortByUseValue(cards)
	return cards[1]:getEffectiveId()
end
sgs.ai_skill_playerchosen["citan"] = function(self, targets)
	local friends = sgs.QList2Table(targets)
	self:sort(friends, "handcard")
	for _, friend in ipairs(friends) do
		if self:isFriend(friend) then
			return friend
		end
	end
	return targets[1]
end

-- lingdi
local lingdi_skill={}
lingdi_skill.name = "lingdi"
table.insert(sgs.ai_skills, lingdi_skill)
lingdi_skill.getTurnUseCard = function(self)
	if self.player:hasUsed("LingdiCard") or self.player:isNude() then return end
	local cards = self.player:getCards("he")
	cards=sgs.QList2Table(cards)
	self:sortByUseValue(cards, true)
	return sgs.Card_Parse("@LingdiCard=" .. cards[1]:getEffectiveId())
end
sgs.ai_skill_use_func["LingdiCard"]=function(card,use,self)
	local up, down
	for _, enemy in ipairs(self.enemies) do
		if enemy:faceUp() then
			up = enemy
			break
		end
	end
	for _, friend in ipairs(self.friends) do
		if not friend:faceUp() then
			down = friend
			break
		end
	end
	if up and down then
		if use.to then
			use.to:append(up)
			use.to:append(down)
		end
		use.card=card
	end
end

-- qiaodou
sgs.ai_skill_invoke["qiaodou"] = function(self, data)
	local sb = data:toPlayer()
	return sb:faceUp() and self:isEnemy(sb)
end

-- cihu
sgs.ai_skill_invoke["@cihu"] = function(self, prompt)
	local num = self.player:getMark("CihuNum")
	local ogami = self.player:getTag("CihuOgami"):toPlayer()
	if self:isFriend(ogami) then return "." end
	local caninvoke = false
	local women = {}
	local players = self.room:getMenorWomen("female")
	players = sgs.QList2Table(players)
	for _, woman in ipairs(players) do
		if woman:isWounded() and self:isFriend(woman) then
			caninvoke = true
			table.insert(women, woman)
		end
	end
	if self.player:getHp() > 2 and caninvoke then
		self:sort(women, "hp")
		local cards = self.player:getCards("he")
		cards = sgs.QList2Table(cards)
		local card_ids = {}
		for i = 1, num do
			table.insert(card_ids, cards[i]:getEffectiveId())
		end
		return "@CihuCard=" .. table.concat(card_ids, "+") .. "->" .. women[1]:objectName()
	end
	return "."
end

