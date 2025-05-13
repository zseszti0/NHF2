#include "gtest_lite.h"

#include "UIAndDisplay.h"
#include "combat.h"
#include "fight.h"

void dmgTest() {
    Sprite c1Sprite("c1","valami");
    Character c1("c1",&c1Sprite,9,10000,10,5,390);

    Sprite c2Sprite("c2","valami");
    Character c2("c2",&c1Sprite,9,10000,10,5,400);

    Sprite c3Sprite("c3","valami");
    Character c3("c3",&c1Sprite,9,10000,10,5,180,1,1,1,1,100,true);

    Enemy e1("e1","valami");
    Enemy e2("e2","valami",10);

    Fight fight("test","valami",&c1,&c2,&c3);
    fight.AddEnemy(&e1);
    fight.AddEnemy(&e2);
    TEST(DMG,FIGHT){
        double enemyHpBefore = e1.GetStat("hp");

        fight.CountNextAction();

        EXPECT_STREQ("c2",fight.currentlyActing->GetName().c_str());

        fight.Normal(Buster);
        EXPECT_NE(enemyHpBefore,e1.GetStat("hp"));
    } END
    TEST(TARGET,FIGHT) {
        double enemyHpBefore = e2.GetStat("hp");
        fight.ChangeEnemyTarget(1);

        fight.Normal(Buster);
        EXPECT_NE(enemyHpBefore,e2.GetStat("hp"));
    } END
    TEST(AOE,FIGHT) {
        double enemyHpBefore = e1.GetStat("hp");

        fight.Normal(Buster);
        EXPECT_EQ(enemyHpBefore,e1.GetStat("hp"));
    } END
    TEST(SP,FIGHT) {
        int spBefore = fight.skillPoints;

        fight.Skill(1);
        EXPECT_NE(spBefore,fight.skillPoints);

        spBefore = fight.skillPoints;
        fight.Normal(Buster);
        EXPECT_NE(spBefore,fight.skillPoints);

    } END
}

void mobTest() {
    Sprite c1Sprite("c1","valami");
    Character c1("c1",&c1Sprite,9,1000,10,5,200);
    Enemy e1("e1","valami");

    TEST(BUFF,CHARACTER) {
        double atkBefore = c1.GetStat("atk");

        c1.GetBuff({"atk",10,3});
        EXPECT_EQ(atkBefore + 10,c1.GetStat("atk"));
    } END
    TEST(HPBUFF,CHARACTER) {

        c1.GetBuff({"hp",-1000000000000,1});
        EXPECT_EQ(1,c1.GetStat("hp"));
    } END
    TEST(OVERHEAL,CHARACTER) {

        c1.SetHP(1000000000000);
        EXPECT_EQ(c1.GetStat("maxhp"),c1.GetStat("hp"));
    } END
    TEST(DEATH,CHARACTER) {

        c1.SetHP(1000000000000);
        EXPECT_EQ(c1.GetStat("maxhp"),c1.GetStat("hp"));
    } END
    TEST(ATK,ENEMY) {
        double chHpBefore = c1.GetStat("hp");
        e1.Atk1(&c1);
        EXPECT_NE(chHpBefore,c1.GetStat("hp"));
    } END
}

int main() {
    GTINIT(std::cin);
    dmgTest();
    mobTest();

    return 0;
}