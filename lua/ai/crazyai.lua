function SmartAI:crazyAI(event, player, data)
	player = player or self.player
	if player:getState() ~= "robot" then return end
	if event == sgs.SlashEffect then -- ɱ��ʱ�����һ�����Ѳ�һ����
		if math.random(0, 3) == 1 and #self.friends_noself > 0 then
			self.friends_noself[1]:drawCards(1)
		end
	elseif event == sgs.SlashHit then -- ɱ��Ŀ��ʱ�����öԷ�һ���ƣ�Ȼ��һ����
		if math.random(0, 3) == 1 then
			local effect = data:toSlashEffect()
			if not effect.to:isNude() then
				local cards = sgs.QList2Table(effect.to:getCards("he"))
				self:sortByUseValue(cards)
				effect.from:obtainCard(cards[1])
				cards = sgs.QList2Table(effect.from:getCards("he"))
				self:sortByUseValue(cards, true)
				effect.to:obtainCard(cards[1])
			end
		end
	elseif event == sgs.PreDeath then -- ɱ��һ��ǰ�������ƺ���������
		local damage = data:toDamage()
		if damage and damage.from then
			local x = damage.from:getMaxCards() - damage.from:getHandcardNum()
			if x > 0 then damage.from:drawCards(x) end
			local recover = sgs.RecoverStruct()
			recover.recover = damage.from:getLostHp()
			recover.card = damage.card
			recover.who = damage.to
			self.room:recover(damage.from, recover)
		end
	elseif event == sgs.DamageProceed then -- ��Ů�Գ�ɱ���+1��-1
		local damage = data:toDamage()
		if damage.to:getGeneral():isFemale() then
			local dt = math.random(0, 2) - 1
			damage.damage = damage.damage + dt
			data:setValue(damage)
		end
	elseif event == sgs.DamageComplete then -- �ܵ��˺����Ը���
		if math.random(0, 2) == 1 then
			local damage = data:toDamage()
			damage.from = data:toDamage().to
			damage.to = data:toDamage().from
			self.room:damage(damage)
		end
	elseif event == sgs.FinishJudge then -- �ж��������һ��������ɫ����һ�λ���
		if math.random(0, 2) == 1 then
			sgs.ai_skill_use["@@huanshu"](self, "@huanshu")
		end
	elseif event == sgs.Pindian then -- ƴ����һ����
		local pindian = data:toPindian()
		pindian.from:drawCards(1)
	elseif event == sgs.MaxHpChanged then -- ���޸ı�ʱ��������
		if math.random(0, 2) == 1 then
			local players = sgs.QList2Table(self.room:getPlayers())
			self:sort(players)
			for _, dead in ipairs(players) do
				if dead:isDead() then
					dead:setRole(player:getRole())
					self.room:revivePlayer(dead)
					self.room:broadcastProperty(dead, "role")
					break
				end
			end
		end
	elseif event == sgs.PhaseChange then
		if player:getPhase() == sgs.Player_Finish then -- �غϽ����׶������һ�����˷���
			if math.random(0, 2) == 1 and #self.enemies > 0 then
				self.enemies[1]:turnOver()
			end
		elseif player:getPhase() == sgs.Player_Start then -- �غϿ�ʼ�׶������һ����������
			if math.random(0, 3) == 1 and #self.enemies > 0 then
				self.enemies[1]:setChained(true)
				self.room:broadcastProperty(self.enemies[1], "chained")
				self.room:setEmotion(self.enemies[1], "chain")
			end
		elseif player:getPhase() == sgs.Player_Play then -- ���ƽ׶���������������ƽ׶�Ч��
			if math.random(0, 2) == 1 then
				player:skip(sgs.Player_Discard)
			end
		end
	end
end
