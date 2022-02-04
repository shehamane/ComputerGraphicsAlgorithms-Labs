package main

import (
	"github.com/go-gl/gl/v2.1/gl"
	"github.com/go-gl/glfw/v3.3/glfw"
	"math/rand"
	"runtime"
	"time"
)

func gen_rand_vertex() {
	lastVertex[0] = lastVertex[2]
	lastVertex[1] = lastVertex[3]
	lastVertex[2] = lastVertex[4]
	lastVertex[3] = lastVertex[5]
	lastVertex[4] = rand.Float32()*2 - 1
	lastVertex[5] = rand.Float32()*2 - 1
}

func draw_triangle() {
	gl.Begin(gl.TRIANGLES)
	gl.Color3f(rand.Float32(), rand.Float32(), rand.Float32())
	gl.Vertex2f(lastVertex[0], lastVertex[1])
	gl.Color3f(rand.Float32(), rand.Float32(), rand.Float32())
	gl.Vertex2f(lastVertex[2], lastVertex[3])
	gl.Color3f(rand.Float32(), rand.Float32(), rand.Float32())
	gl.Vertex2f(lastVertex[4], lastVertex[5])
	gl.End()
}

var lastVertex [6]float32
var isRunning bool = false
var isNew bool = false

func main() {
	lastVertex[0] = 0
	lastVertex[1] = 0
	lastVertex[2] = 0.5
	lastVertex[3] = 0.5
	lastVertex[4] = 0
	lastVertex[5] = -0.5

	runtime.LockOSThread()
	rand.Seed(time.Now().UnixNano())

	err := glfw.Init()
	if err != nil {
		panic(err)
	}
	defer glfw.Terminate()

	glfw.WindowHint(glfw.Resizable, glfw.False)
	window, err := glfw.CreateWindow(1280, 720, "MDMA", nil, nil)
	if err != nil {
		panic(err)
	}
	window.MakeContextCurrent()

	err = gl.Init()
	if err != nil {
		panic(err)
	}

	window.SetMouseButtonCallback(glfw.MouseButtonCallback(func(_ *glfw.Window, button glfw.MouseButton, action glfw.Action, mods glfw.ModifierKey) {
		if action == glfw.Press {
			if button == glfw.MouseButtonLeft {
				isRunning = !isRunning
			}
			if button == glfw.MouseButtonRight {
				isNew = true
			}
		}
	}))

	for !window.ShouldClose() {
		if isRunning || isNew {
			if isNew {
				isNew = false
			}
			draw_triangle()
			gen_rand_vertex()

			window.SwapBuffers()
		}
		glfw.PollEvents()
	}
}
