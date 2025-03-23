package main

import (
	rl "github.com/gen2brain/raylib-go/raylib"
	"log"
	"os"
	"strings"
)

const (
	width = 5
	border = 5
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

type Vector2Int struct {
	X int
	Y int
}

func fromPos(x, y int) rl.Vector2{
	return rl.Vector2{border + float32(x) * width, border + float32(y) * width}
}

func fromPosOrient(x, y int, dir int) (rl.Vector2, rl.Vector2, rl.Vector2){
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

func advance(cursor Vector2Int, dir int) Vector2Int {
	switch (dir) {
		case right: return Vector2Int{cursor.X + 1, cursor.Y}
		case left: return Vector2Int{cursor.X - 1, cursor.Y}
		case up: return Vector2Int{cursor.X, cursor.Y - 1}
		case down: return Vector2Int{cursor.X, cursor.Y + 1}
	}
	log.Fatal("bad dir %d", dir)
	return cursor
}

func main() {
	rl.InitWindow(1400, 1000, "AOC day22")
	defer rl.CloseWindow()
	rl.SetTargetFPS(60)

	data, err := os.ReadFile("./day22_in")
	if err != nil {
		log.Fatal(err)
	}
	block := string(data)
	blocks := strings.Split(block, "\n\n")
	lines := strings.Split(blocks[0], "\n")

	log.Println(blocks[1])

	cursor := Vector2Int{}
	outer:
	for i := range len(lines) - 1 {
		for j := range len(lines[i]) {
			c := lines[i][j]
			if (c == '.') {
				cursor.X = j;
				cursor.Y = i;
				break outer;
			}
		}
	}
	log.Printf("cursor x %f y %f\n", cursor.X, cursor.Y)

	dir := right

	for !rl.WindowShouldClose() {
		if rl.IsKeyPressed(rl.KeyRight) {
			dir = rotate(dir, turnClock)
		} else if rl.IsKeyPressed(rl.KeyLeft) {
			dir = rotate(dir, turnCounterClock)
		} else if rl.IsKeyPressed(rl.KeySpace) {
			candidateCursor := advance(cursor, dir)
			if candidateCursor.Y >= 0 && candidateCursor.Y < len(lines) {
				if candidateCursor.X >= 0 && candidateCursor.X < len(lines[candidateCursor.X]) {
					if lines[candidateCursor.Y][candidateCursor.X] == '.' {
						cursor = candidateCursor
					}
				}
			}
		}

		rl.BeginDrawing()
		rl.ClearBackground(rl.RayWhite)
		for i := range len(lines) - 1 {
			for j := range len(lines[i]) {
				c := lines[i][j]
				x := int32(border + j * width)
				y := int32(border + i * width)
				switch(c) {
				case ' ':
					rl.DrawRectangle(x, y, width, width, rl.RayWhite)
				case '.':
					rl.DrawRectangle(x, y, width, width, rl.Blue)
				case '#':
					rl.DrawRectangle(x, y, width, width, rl.Black)
				default:
					log.Fatalf("Unrecognised char '%c'", c)
				}
			}
		}
		v1, v2, v3 := fromPosOrient(cursor.X, cursor.Y, dir)
		rl.DrawTriangle(v1, v2, v3, rl.Red)

		rl.EndDrawing()
	}
}

