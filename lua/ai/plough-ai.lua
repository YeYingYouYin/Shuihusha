-- this scripts contains the AI classes for generals of plough package

sgs.ai_skill_invoke.double_whip = function(self, data)
	local carduse = data:toCardUse()
	local tos = sgs.QList2Table(carduse.to)
	local mengkang = self.room:findPlayerBySkillName("mengchong")
	local mk = mengkang and self:isFriend(mengkang)
	for _, target in ipairs(tos) do
		if target:isChained() then
			return self:isFriend(target) and not mk
		else
			return self:isEnemy(target) and not mk
		end
		break
	end
end

sgs.weapon_range.DoubleWhip = 2
sgs.weapon_range.MeteorSword = 3
sgs.weapon_range.SunBow = 5

function SmartAI:searchForEcstasy(use,enemy,slash)
    if not self.toUse then return nil end

	for _,card in ipairs(self.toUse) do
		if card:getId()~= slash:getId() then return nil end
	end

	if not use.to then return nil end
	if self.player:hasUsed("Ecstasy") then return nil end

	local cards = self.player:getHandcards()
	cards = sgs.QList2Table(cards)
	self:fillSkillCards(cards)

	if sgs.getDefense(self.player) < sgs.getDefense(enemy) and
		self.player:getHandcardNum() < self.player:getHp() + 1 then
			return
	end

	if not self.player:canSlash(enemy) or enemy:hasFlag("ecst") then
		return
	end

	local card_str = self:getCardId("Ecstasy")
	if card_str then return sgs.Card_Parse(card_str) end

	for _, mi in ipairs(cards) do
		if (mi:className() == "Ecstasy") and not (mi:getEffectiveId() == slash:getEffectiveId()) and
			not isCompulsoryView(mi, "Slash", self.player, sgs.Player_Hand) then
			return mi
		end
	end
end

sgs.dynamic_value.control_card.Ecstasy = true
sgs.dynamic_value.benefit.Counterplot = true

-- bi shang liang shan
function SmartAI:useCardDrivolt(drivolt, use)
--	if self.player:hasSkill("wuyan") then return end
	use.card = drivolt
	self:sort(self.enemies, "hp")
	if self.enemies[1]:getHp() == 1 and self.enemies[1]:getKingdom() ~= self.player:getKingdom() then
		if use.to then use.to:append(self.enemies[1]) end
		return
	end
	for _, friend in ipairs(self.friends_noself) do
		if not friend:isWounded() and friend:getKingdom() ~= self.player:getKingdom() then
			if use.to then use.to:append(friend) end
			return
		end
	end
	for _, enemy in ipairs(self.enemies) do
		if enemy:getHp() == 2 and enemy:getKingdom() ~= self.player:getKingdom() then
			if use.to then use.to:append(enemy) end
			return
		end
	end
	local players = {}
	for _, player in ipairs(sgs.QList2Table(self.room:getOtherPlayers(self.player))) do
		if player:getKingdom() ~= self.player:getKingdom() then table.insert(players, player) end
	end
	if #players < 1 then
		use.card = nil
		return "."
	end
	local r = math.random(1, #players)
	if use.to then use.to:append(players[r]) end
end

sgs.dynamic_value.damage_card.Drivolt = true
sgs.dynamic_value.control_card.Drivolt = true

-- tan ting
function SmartAI:useCardWiretap(wiretap, use)
--	if self.player:hasSkill("wuyan") then return end
	local targets = {}
	if #self.friends_noself > 0 then
		self:sort(self.friends_noself, "handcard")
		table.insert(targets, self.friends_noself[#self.friends_noself])
	end
	if #self.enemies > 0 then
		self:sort(self.enemies, "handcard")
		table.insert(targets, self.enemies[#self.enemies])
	end
	use.card = wiretap
	if use.to then
		local r = math.random(1, 2)
		if targets[r] ~= self.player then
			use.to:append(targets[r])
		else
			return "."
		end
	end
end

sgs.dynamic_value.benefit.Wiretap = true

-- xing ci
function SmartAI:useCardAssassinate(ass, use)
--	if self.player:hasSkill("wuyan") then return end
	if not self.enemies[1] then return end
	for _, enemy in ipairs(self.enemies) do
		if (enemy:hasSkill("fushang") and enemy:getHp() > 3) or enemy:hasSkill("huoshui") then
			use.card = ass
			if use.to then
				use.to:append(enemy)
			end
			return
		end
	end
	self:sort(self.enemies, "hp")
	local target
	for _, enemy in ipairs(self.enemies) do
		if enemy:hasFlag("ecst") or
			(not self:isEquip("EightDiagram", enemy) and enemy:getHandcardNum() < 6) then
			target = enemy
		end
	end
	use.card = ass
	if use.to then
		if target then
			use.to:append(target)
		else
			use.to:append(self.enemies[1])
		end
	end
end

sgs.dynamic_value.damage_card.Assassinate = true

-- sheng chen gang
function SmartAI:useCardTreasury(card, use)
	if not self.player:containsTrick("treasury") then
		use.card = card
	end
end

sgs.dynamic_value.lucky_chance.Treasury = true

-- hai xiao
function SmartAI:useCardTsunami(card, use)
	if self.player:containsTrick("tsunami") then return end
--	if self.player:hasSkill("weimu") and card:isBlack() then return end

	if not self:hasWizard(self.enemies) then--and self.room:isProhibited(self.player, self.player, card) then
		if self:hasWizard(self.friends) then
			use.card = card
			return
		end
		local players = self.room:getAllPlayers()
		players = sgs.QList2Table(players)

		local friends = 0
		local enemies = 0

		for _,player in ipairs(players) do
			if self:objectiveLevel(player) >= 4 then
				enemies = enemies + 1
			elseif self:isFriend(player) then
				friends = friends + 1
			end
		end

		local ratio

		if friends == 0 then ratio = 999
		else ratio = enemies/friends
		end

		if ratio > 1.5 then
			use.card = card
			return
		end
	end
end

sgs.dynamic_value.lucky_chance.Tsunami = true

-- ji cao tun liang
function SmartAI:useCardProvistore(provistore, use)
--	if self.player:hasSkill("wuyan") then return end
	use.card = provistore
	for _, friend in ipairs(self.friends) do
		if not friend:containsTrick("provistore") and friend:getHandcardNum() > 3 then
			if use.to then
				use.to:append(friend)
			end
			return
		end
	end
	self:sort(self.friends, "hp")
	if use.to then
		use.to:append(self.friends[1])
	end
end

sgs.dynamic_value.control_usecard.Provistore = true
sgs.dynamic_value.benefit.Provistore = true

