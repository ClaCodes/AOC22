package main

import (
	rl "github.com/gen2brain/raylib-go/raylib"
	"log"
	"os"
	"strings"
)

const (
	width = 4
)

const (
	up = iota
	left
	down
	right
)

const (
	turnClock = iota
	turnCounterClock
)

type Cursor struct {
	X int
	Y int
	dir int
}

func fromPos(x, y int) rl.Vector2{
	return rl.Vector2{float32(x) * width, float32(y) * width}
}

func toTriangle(c Cursor) (rl.Vector2, rl.Vector2, rl.Vector2){
	x := c.X
	y := c.Y
	dir := c.dir
	c1 := fromPos(x, y)
	c2 := fromPos(x + 1, y)
	c3 := fromPos(x + 1, y + 1)
	c4 := fromPos(x, y + 1)
	switch(dir) {
	case right: return c1, c4, rl.Vector2{c2.X, c2.Y + width/2}
	case left: return c3, c2, rl.Vector2{c1.X, c1.Y + width/2}
	case down: return c2, c1, rl.Vector2{c4.X + width/2, c4.Y}
	case up: return c4, c3, rl.Vector2{c1.X + width/2, c1.Y}
	default: log.Fatalf("bad dir %d", dir)
	}
	return c1, c2, c3
}

func rotate(dir int, turn int) int {
	if (turn == turnClock) {
		switch (dir) {
			case right: return down
			case left: return up
			case up: return right
			case down: return left
		}
	} else {
		switch (dir) {
			case right: return up
			case left: return down
			case up: return left
			case down: return right
		}
	}
	log.Fatal("bad turn %d", turn)
	return right
}

func inBound(cursor Cursor, lines []string) bool {
	if cursor.Y >= 0 && cursor.Y < len(lines) {
		if cursor.X >= 0 && cursor.X < len(lines[cursor.Y]) {
			return true
		}
	}
	return false
}

func advance(cursor Cursor) Cursor {
	switch (cursor.dir) {
		case right: return Cursor{cursor.X + 1, cursor.Y, cursor.dir}
		case left: return Cursor{cursor.X - 1, cursor.Y, cursor.dir}
		case up: return Cursor{cursor.X, cursor.Y - 1, cursor.dir}
		case down: return Cursor{cursor.X, cursor.Y + 1, cursor.dir}
	}
	log.Fatal("bad dir %d", cursor.dir)
	return cursor
}

func revert(cursor Cursor) Cursor {
	switch (cursor.dir) {
		case right: return Cursor{cursor.X - 1, cursor.Y, cursor.dir}
		case left: return Cursor{cursor.X + 1, cursor.Y, cursor.dir}
		case up: return Cursor{cursor.X, cursor.Y + 1, cursor.dir}
		case down: return Cursor{cursor.X, cursor.Y - 1, cursor.dir}
	}
	log.Fatal("bad dir %d", cursor.dir)
	return cursor
}

func part1Portals(in Cursor, lines []string) Cursor {
	candidate := in
	for {
		candidate = revert(candidate)
		if (!inBound(candidate, lines) || lines[candidate.Y][candidate.X] == ' ') {
			candidate = advance(candidate)
			break
		}
	}
	return candidate
}

func part2Portals(in Cursor, lines []string, portals map[Cursor]Cursor) Cursor {
	s, ok := portals[in]
	if ok {
		return s
	}
	if in.X >= 50 && in.X < 100 && in.Y == 0 && in.dir == up {
		return Cursor{in.Y, in.X + 100 , right}
	} else if in.X >= 100 && in.X < 150 && in.Y == 0 && in.dir == up {
		return Cursor{in.X-100, in.Y + 199, up}
	} else if in.X == 50 && in.Y >= 0 && in.Y < 50 && in.dir == left {
		return Cursor{in.X - 50, 149-in.Y, right}
	} else if in.X == 149 && in.Y >= 0 && in.Y < 50 && in.dir == right {
		return Cursor{in.X - 50, 149 - in.Y, left}
	} else if in.X >= 100 && in.X < 150 && in.Y == 49 && in.dir == down {
		return Cursor{in.Y + 50, in.X - 50, left}
	} else if in.X == 50 && in.Y >= 50 && in.Y < 100 && in.dir == left {
		return Cursor{in.Y - 50, in.X + 50, down}
	} else if in.X >= 50 && in.X < 100 && in.Y == 149 && in.dir == down {
		return Cursor{in.Y-100, in.X + 100 , left}
	} else {
		// log.Fatal(in, portals)
		// log.Fatal(in)
	}	
	return Cursor{0,0,right}
}

func rotate180(i int) int {
	switch(i) {
		case up: return down
		case down: return up
		case right: return left
		case left: return right
	}
	log.Fatal("bad dir")
	return up
}

func shift(c Cursor) Cursor {
	c = revert(c)
	c.dir = rotate180(c.dir)
	return c
}

type Comands struct {
	turn int
	steps int
}

func convert(s string) []Comands {
	log.Println(s)
	cmds := make([]Comands, 0)
	c := 0
	for pos, char := range s {
		switch (char) {
		case '\n':
		case '0': c*=10 ; c+= 0
		case '1': c*=10 ; c+= 1
		case '2': c*=10 ; c+= 2
		case '3': c*=10 ; c+= 3
		case '4': c*=10 ; c+= 4
		case '5': c*=10 ; c+= 5
		case '6': c*=10 ; c+= 6
		case '7': c*=10 ; c+= 7
		case '8': c*=10 ; c+= 8
		case '9': c*=10 ; c+= 9
		case 'R': cmds = append(cmds, Comands{turnClock, c}); c = 0;
		case 'L': cmds = append(cmds, Comands{turnCounterClock, c}); c = 0;
		default:
			log.Fatalf("bad character %c starts at byte position %d\n", char, pos)
		}
	}
	cmds = append(cmds, Comands{turnCounterClock, c})
	return cmds
}

const screenWidth = 1400
const screenHeight = 1000

func main() {
	rl.SetConfigFlags(rl.FlagWindowHighdpi)
	// rl.SetConfigFlags(rl.FlagWindowResizable)
	rl.InitWindow(screenWidth, screenHeight, "AOC day22")
	defer rl.CloseWindow()
	rl.SetTargetFPS(60)

	data, err := os.ReadFile("./day22_in")
	if err != nil {
		log.Fatal(err)
	}
	block := string(data)
	blocks := strings.Split(block, "\n\n")
	lines := strings.Split(blocks[0], "\n")

	program := convert(blocks[1])
	log.Println(program)

	portals := make(map[Cursor]Cursor)
	visited := make(map[Cursor]struct{})
	portalColors := make(map[Cursor]rl.Color)
	counter := float32(0.0)
	for i := range len(lines) {
		for j := range len(lines[i]) {
			for dir := range 4 {
			    candidate := Cursor{j, i, dir}
				next := advance(candidate)
				c := lines[i][j]
				if (!inBound(next, lines) || lines[next.Y][next.X] == ' ') && c == '.' {
					candidate = part2Portals(candidate, lines, portals)
					// candidate = part1Portals(candidate, lines)
					if (inBound(candidate, lines) && lines[candidate.Y][candidate.X] == '.') {
						portals[next] = candidate
						portalColors[next] = rl.ColorFromHSV(counter, 0.5, 1.0)
						portals[shift(candidate)] = shift(next)
						portalColors[shift(candidate)] = rl.ColorFromHSV(counter, 0.5, 1.0)
						counter += 1.0
					}
				}
			}
		}
	}

	cursor := Cursor{dir:right}
	outer:
	for i := range len(lines) {
		for j := range len(lines[i]) {
			c := lines[i][j]
			if (c == '.') {
				cursor.X = j;
				cursor.Y = i;
				break outer;
			}
		}
	}

	zoom := float32(1.0)
	for !rl.WindowShouldClose() {
		if rl.IsKeyPressed(rl.KeyRight) {
			cursor.dir = rotate(cursor.dir, turnClock)
		} else if rl.IsKeyPressed(rl.KeyLeft) {
			cursor.dir = rotate(cursor.dir, turnCounterClock)
		} else if rl.IsKeyDown(rl.KeySpace) || rl.IsKeyPressed(rl.KeyS) {
			candidateCursor := advance(cursor)
			log.Printf("candidateCursor x %d y %d\n", candidateCursor.X, candidateCursor.Y)
			c, ok := portals[candidateCursor]
			if (ok) {
				cursor = c
			} else if inBound(candidateCursor, lines) {
				if lines[candidateCursor.Y][candidateCursor.X] == '.' {
					cursor = candidateCursor
				}
			}
		} else if rl.IsKeyDown(rl.KeyC) || rl.IsKeyPressed(rl.KeyN) {
			for range 100 {
			if len(program) == 0 {
				log.Println("done", cursor)
				log.Println("done", (cursor.Y + 1) * 1000 + 4 * (cursor.X + 1))
			} else if program[0].steps == 0 {
				var done Comands
				done, program = program[0], program[1:]
				if (len(program) > 0) {
					cursor.dir = rotate(cursor.dir, done.turn)
				}
			} else {
				program[0].steps -= 1
				candidateCursor := advance(cursor)
				log.Printf("%v candidateCursor x %d y %d\n", len(program), candidateCursor.X, candidateCursor.Y)
				c, ok := portals[candidateCursor]
				if (ok) {
					cursor = c
				} else if inBound(candidateCursor, lines) {
					if lines[candidateCursor.Y][candidateCursor.X] == '.' {
						cursor = candidateCursor
					}
				}
			}
		visited[cursor] = struct{}{}
			}
		}
		if rl.IsKeyPressed(rl.KeyZ) {
			if zoom < 2.0 {
				zoom = 5.0
			} else {
				zoom = 1.0
			}
		}

		visited[cursor] = struct{}{}

		rl.BeginDrawing()
		rl.ClearBackground(rl.Black)
		camera := rl.Camera2D{
			Offset: rl.Vector2{screenWidth, screenHeight},
			Target: fromPos(cursor.X, cursor.Y),
			Rotation: 0.0,
			Zoom: zoom,
		}
		rl.BeginMode2D(camera)
		for i := range len(lines) {
			for j := range len(lines[i]) {
				c := lines[i][j]
				y := int32(i * width)
				x := int32(j * width)
				switch(c) {
				case ' ':
					rl.DrawRectangle(x, y, width, width, rl.Black)
				case '.':
					been := false
					for dir := range 4 {
						_, ok := visited[Cursor{j, i, dir}]
						if ok {
							been = true
							break
						}
					}
					if been {
						rl.DrawRectangle(x, y, width, width, rl.Green)
					} else {
						rl.DrawRectangle(x, y, width, width, rl.Blue)
					}
				case '#':
					rl.DrawRectangle(x, y, width, width, rl.DarkBlue)
				default:
					log.Fatalf("Unrecognised char '%c'", c)
				}
			}
		}
		for k, v := range portals {
			c := portalColors[k]
			v1, v2, v3 := toTriangle(k)
			rl.DrawTriangle(v1, v2, v3, c)
			v1, v2, v3 = toTriangle(v)
			rl.DrawTriangle(v1, v2, v3, c)
		}
		v1, v2, v3 := toTriangle(cursor)
		rl.DrawTriangle(v1, v2, v3, rl.Red)


		rl.EndMode2D()
		rl.EndDrawing()
	}
	log.Println("visited", len(visited))
}

