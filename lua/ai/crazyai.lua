function SmartAI:crazyAI(event, player, data)
	player = player or self.player
	if event == sgs.SlashEffect then -- ɱ��ʱ�����һ�����Ѳ�һ����
		if math.random(0, 3) == 1 and #self.friends_noself > 0 then
			self.friends_noself[1]:drawCards(1)
		end
	elseif event == sgs.SlashProceed then
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
	elseif event == sgs.Death then --ɱ��һ���˲������ƺ���������
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
	elseif event == sgs.Damaged then --�ܵ��˺����Ը���
		if math.random(0, 3) == 1 then
			local damage = data:toDamage()
			local damage2 = damage
			damage2.to = damage.from
			damage2.from = damage.to
			self.room:damage(damage2)
		end
	elseif event == sgs.FinishJudge then --�ж��������һ��������ɫ����һ�λ���
		if math.random(0, 3) == 1 then
			self:askForUseCard("@@huanshu", "@huanshu")
		end
	elseif event == sgs.Pindian then --ƴ��ǰ��һ����
		local pindian = data:toPindian()
		pindian.from:drawCards(1)
	elseif event == sgs.PhaseChange then
		if player:getPhase() == sgs.Player_Finish then -- �غϽ����׶������һ�����˷���
			if math.random(0, 3) == 1 and #self.enemies > 0 then
				self.enemies[1]:turnOver()
			end
		elseif player:getPhase() == sgs.Player_Play then -- ���ƽ׶���������������ƽ׶�Ч��
			if math.random(0, 3) == 1 then
				player:skip(sgs.Player_Discard)
			end
		end
	end
end
