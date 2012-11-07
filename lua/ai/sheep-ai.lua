-- zhiqu
sgs.ai_view_as["zhiqu"] = function(card, player, card_place)
	local suit = card:getSuitString()
	local number = card:getNumberString()
	local card_id = card:getEffectiveId()
	if card:inherits("EquipCard") then
		return ("nullification:zhiqu[%s:%s]=%d"):format(suit, number, card_id)
	end
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

-- bingji
local bingji_skill={}
bingji_skill.name = "bingji"
table.insert(sgs.ai_skills, bingji_skill)
bingji_skill.getTurnUseCard = function(self)
	if not self:slashIsAvailable() or not self.player:isWounded() then return end
	local first_found = false
	local second_found = false
	local first_card, second_card
	if self.player:getHandcardNum() >= 2 then
		local cards = self.player:getHandcards()
		cards = sgs.QList2Table(cards)
		for _, fcard in ipairs(cards) do
			if not (fcard:inherits("Peach") or fcard:inherits("ExNihilo") or fcard:inherits("AOE")) then
				first_card = fcard
				first_found = true
				for _, scard in ipairs(cards) do
					if first_card ~= scard and scard:getType() == first_card:getType() and 
						not (scard:inherits("Peach") or scard:inherits("ExNihilo") or scard:inherits("AOE")) then
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
		return sgs.Card_Parse("@BingjiCard=" .. first_card:getId() + second_card:getId())
	end
end
sgs.ai_skill_use_func["BingjiCard"] = function(card, use, self)
	local targetnum = self.player:getLostHp()
	self:sort(self.enemies, "defense")
	local a = 0
	for _, enemy in ipairs(self.enemies) do
		if use.to then
			use.to:append(target)
			a = a + 1
		end
		if a == 2 then
			use.card = card
			return
		end
	end
end

-- qiaojiang
local qiaojiang_skill={}
qiaojiang_skill.name = "qiaojiang"
table.insert(sgs.ai_skills, qiaojiang_skill)
qiaojiang_skill.getTurnUseCard = function(self)
	local cards = self.player:getCards("h")
	cards=sgs.QList2Table(cards)
	local jink_card
	self:sortByUseValue(cards, true)
	for _,card in ipairs(cards)  do
		if card:isBlack() and card:inherits("TrickCard") then
			jink_card = card
			break
		end
	end
	if not jink_card then return nil end
	local suit = jink_card:getSuitString()
	local number = jink_card:getNumberString()
	local card_id = jink_card:getEffectiveId()
	local card_str = ("slash:qiaojiang[%s:%s]=%d"):format(suit, number, card_id)
	local slash = sgs.Card_Parse(card_str)
	assert(slash)
	return slash
end
sgs.ai_view_as["qiaojiang"] = function(card, player, card_place)
	local suit = card:getSuitString()
	local number = card:getNumberString()
	local card_id = card:getEffectiveId()

	if card:isBlack() and card:inherits("TrickCard") then
		return ("slash:qiaojiang[%s:%s]=%d"):format(suit, number, card_id)
	elseif card:isRed() and card:inherits("TrickCard") then
		return ("jink:qiaojiang[%s:%s]=%d"):format(suit, number, card_id)
	end
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
			if not fcard:inherits("Peach") and fcard:inherits("BasicCard") then
				first_card = fcard
				first_found = true
				for _, scard in ipairs(cards) do
					if first_card ~= scard and scard:inherits("BasicCard") and
						(scard:isRed() and first_card:isRed()) and 
						not scard:inherits("Peach") then
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
		return assassinate
	end
end

-- yunchou
function sgs.ai_skill_use_func.YunchouCard(card, use)
	local subcard = card:getSubcards():first()
	local newuse = "ex_nihilo|..."
	for _, anewuse in ipairs(newuse:split("|")) do
		local newusecard = sgs.Card_Parse(("%s:yunchou[%s:%s]=%d"):format(
			anewuse, subcard:getSuitString(), subcard:getNumberString(), subcard:getEffectiveId()))
		self:useTrickCard(newusecard, use)
		if use.card then return end
	end
	use.card = nil
end
