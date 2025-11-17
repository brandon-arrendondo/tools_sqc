/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: game_constants.c
 *
 * This case demonstrates violations where game configuration
 * constants and parameters are not const-qualified.
 */

#include <stdio.h>

void player_stats(void) {
    /* NON-COMPLIANT: Player stat limits should be const */
    int MAX_HEALTH = 100;
    int MAX_MANA = 50;
    int MAX_EXPERIENCE = 999999;
    int MAX_LEVEL = 99;
    int MAX_INVENTORY_SIZE = 30;

    /* NON-COMPLIANT: Starting values should be const */
    int STARTING_HEALTH = 100;
    int STARTING_MANA = 20;
    int STARTING_LEVEL = 1;
    int STARTING_EXPERIENCE = 0;
    int STARTING_GOLD = 100;

    printf("Player Statistics:\\n");
    printf("  Max values: Health=%d, Mana=%d, Level=%d\\n",
           MAX_HEALTH, MAX_MANA, MAX_LEVEL);
    printf("  Starting values: Health=%d, Mana=%d, Level=%d, Gold=%d\\n",
           STARTING_HEALTH, STARTING_MANA, STARTING_LEVEL, STARTING_GOLD);

    /* Stats used for player initialization but never modified */
    int player_health = STARTING_HEALTH;
    int player_level = STARTING_LEVEL;
    printf("  New player: Level %d, Health %d\\n", player_level, player_health);
}

void world_dimensions(void) {
    /* NON-COMPLIANT: World size constants should be const */
    int WORLD_WIDTH = 1000;
    int WORLD_HEIGHT = 1000;
    int CHUNK_SIZE = 16;
    int MAX_HEIGHT = 256;
    int SEA_LEVEL = 64;

    /* NON-COMPLIANT: Spawn coordinates should be const */
    int SPAWN_X = 500;
    int SPAWN_Y = 500;
    int SPAWN_Z = 70;

    /* NON-COMPLIANT: World generation parameters should be const */
    int TERRAIN_SEED = 12345;
    int CAVE_FREQUENCY = 15;  /* percentage */
    int ORE_DENSITY = 3;      /* percentage */

    printf("\\nWorld Configuration:\\n");
    printf("  Dimensions: %dx%d (chunks: %dx%d)\\n",
           WORLD_WIDTH, WORLD_HEIGHT, WORLD_WIDTH/CHUNK_SIZE, WORLD_HEIGHT/CHUNK_SIZE);
    printf("  Height limits: Sea level=%d, Max height=%d\\n", SEA_LEVEL, MAX_HEIGHT);
    printf("  Spawn point: (%d, %d, %d)\\n", SPAWN_X, SPAWN_Y, SPAWN_Z);
    printf("  Generation: Seed=%d, Caves=%d%%, Ore=%d%%\\n",
           TERRAIN_SEED, CAVE_FREQUENCY, ORE_DENSITY);

    /* World parameters used for generation but never modified */
    int player_x = SPAWN_X;
    int player_y = SPAWN_Y;
    printf("  Player spawned at: (%d, %d)\\n", player_x, player_y);
}

void game_mechanics(void) {
    /* NON-COMPLIANT: Timing constants should be const */
    int TICK_RATE = 20;           /* ticks per second */
    int DAY_LENGTH = 24000;       /* ticks (20 minutes) */
    int NIGHT_LENGTH = 8000;      /* ticks */
    int WEATHER_CHANGE_CHANCE = 1; /* percentage per tick */

    /* NON-COMPLIANT: Physics constants should be const */
    double GRAVITY = 9.8;         /* blocks per second squared */
    double JUMP_VELOCITY = 5.0;   /* blocks per second */
    double WALK_SPEED = 4.3;      /* blocks per second */
    double RUN_SPEED = 5.6;       /* blocks per second */

    /* NON-COMPLIANT: Damage multipliers should be const */
    double CRITICAL_HIT_MULTIPLIER = 1.5;
    double WEAKNESS_MULTIPLIER = 2.0;
    double RESISTANCE_MULTIPLIER = 0.5;
    double FIRE_DAMAGE_PER_TICK = 1.0;

    printf("\\nGame Mechanics:\\n");
    printf("  Timing: %d ticks/sec, Day=%d ticks, Night=%d ticks\\n",
           TICK_RATE, DAY_LENGTH, NIGHT_LENGTH);
    printf("  Physics: Gravity=%.1f, Jump=%.1f, Walk=%.1f, Run=%.1f\\n",
           GRAVITY, JUMP_VELOCITY, WALK_SPEED, RUN_SPEED);
    printf("  Combat: Critical=%.1fx, Weakness=%.1fx, Resistance=%.1fx\\n",
           CRITICAL_HIT_MULTIPLIER, WEAKNESS_MULTIPLIER, RESISTANCE_MULTIPLIER);

    /* Mechanics used for calculations but never modified */
    double damage = 10.0;
    double critical_damage = damage * CRITICAL_HIT_MULTIPLIER;
    printf("  Sample damage: %.1f -> %.1f (critical)\\n", damage, critical_damage);
}

void item_properties(void) {
    /* NON-COMPLIANT: Item rarity levels should be const */
    char rarity_common[] = "Common";
    char rarity_uncommon[] = "Uncommon";
    char rarity_rare[] = "Rare";
    char rarity_epic[] = "Epic";
    char rarity_legendary[] = "Legendary";

    /* NON-COMPLIANT: Item type categories should be const */
    char type_weapon[] = "Weapon";
    char type_armor[] = "Armor";
    char type_consumable[] = "Consumable";
    char type_material[] = "Material";
    char type_tool[] = "Tool";

    /* NON-COMPLIANT: Durability limits should be const */
    int WOODEN_DURABILITY = 100;
    int STONE_DURABILITY = 250;
    int IRON_DURABILITY = 500;
    int DIAMOND_DURABILITY = 1000;
    int NETHERITE_DURABILITY = 2000;

    printf("\\nItem System:\\n");
    printf("  Rarities: %s, %s, %s, %s, %s\\n",
           rarity_common, rarity_uncommon, rarity_rare, rarity_epic, rarity_legendary);
    printf("  Types: %s, %s, %s, %s, %s\\n",
           type_weapon, type_armor, type_consumable, type_material, type_tool);
    printf("  Durability: Wood=%d, Stone=%d, Iron=%d, Diamond=%d, Netherite=%d\\n",
           WOODEN_DURABILITY, STONE_DURABILITY, IRON_DURABILITY,
           DIAMOND_DURABILITY, NETHERITE_DURABILITY);

    /* Item properties used for item creation but never modified */
    char new_item_type[] = "Weapon";
    int new_item_durability = IRON_DURABILITY;
    printf("  Created %s with %d durability\\n", new_item_type, new_item_durability);
}

int main(void) {
    /* NON-COMPLIANT: Game configuration should be const */
    char game_title[] = "Adventure Game";
    char game_version[] = "1.0.0";
    int max_players = 4;
    int autosave_interval = 300;  /* seconds */

    /* NON-COMPLIANT: Difficulty settings should be const */
    char difficulty_easy[] = "Easy";
    char difficulty_normal[] = "Normal";
    char difficulty_hard[] = "Hard";
    char difficulty_nightmare[] = "Nightmare";

    printf("Game Configuration:\\n");
    printf("  Title: %s v%s\\n", game_title, game_version);
    printf("  Max players: %d\\n", max_players);
    printf("  Autosave: every %d seconds\\n", autosave_interval);
    printf("  Difficulties: %s, %s, %s, %s\\n",
           difficulty_easy, difficulty_normal, difficulty_hard, difficulty_nightmare);

    player_stats();
    world_dimensions();
    game_mechanics();
    item_properties();

    return 0;
}