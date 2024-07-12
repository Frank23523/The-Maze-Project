#include "../inc/map.h"

/**
 * parse_map_file - Parses a map file and populates the game map.
 * @game: Pointer to the Game structure where the map will be stored.
 * @file_path: Path to the map file to be parsed.
 * Return: true if parsing and storing the map was successful, false otherwise.
 */
bool parse_map_file(struct Game *game, const char *file_path)
{
	char line[MAP_WIDTH + 2];
	int y, x;
	FILE *file = fopen(file_path, "r");

	if (!file)
	{
		fprintf(stderr, "Error opening map file: %s\n", file_path);
		return (false);
	}

	for (y = 0; y < MAP_HEIGHT; y++)
	{
		if (fgets(line, sizeof(line), file) == NULL)
		{
			fprintf(stderr, "Error reading map file or incorrect format.\n");
			fclose(file);
			return (false);
		}
		for (x = 0; x < MAP_WIDTH; x++)
		{
			if (line[x] == '1')
				game->map[y * MAP_WIDTH + x] = MAP_WALL;
			else if (line[x] == '0')
				game->map[y * MAP_WIDTH + x] = MAP_FLOOR;
			else
			{
				fprintf(stderr, "Invalid character in map file at line %d, column %d.\n",
						y + 1, x + 1);
				fclose(file);
				return (false);
			}
		}
	}

	fclose(file);
	return (true);
}
