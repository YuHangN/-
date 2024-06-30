#include <easyx.h>
#include <deque>

// 画布大小，即会话窗口大小。
#define WIDTH 640
#define HEIGHT 480
// 蛇身一个小方格的像素大小。
#define SIZE 20
// 蛇身的移动速度：游戏难度,这里的意思是每一步0.2秒，200毫秒。
#define DELAY 200
// 蛇移动的方向，可以使用枚举。
enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
};
// 定义每一块蛇身。
typedef struct {
	int x;
	int y;
	Direction dir;
} Snake;
// 定义食物。
typedef struct {
	int x;
	int y;
} Food;

void drawSnake(std::deque<Snake>& snake) {
	// 绿色填充，代表蛇身。
	// This function sets the fill color for subsequent drawing operations to green.
	setfillcolor(GREEN);
	for (auto body : snake) {
		// 绘制矩形框，在屏幕中，原点0为左上角而不是左下角。
		// This function draws a filled rectangle with the specified coordinates,
		fillrectangle(body.x * SIZE, body.y * SIZE, (body.x + 1) * SIZE, (body.y + 1) * SIZE);
	}
	// 控制蛇的移动速度与方向。
	 Sleep(DELAY);
}

// 判断游戏是否结束。 
void checkEnd(std::deque<Snake>& snake) {
	// 检测碰撞
	for (auto iter = snake.begin() + 1; iter != snake.end(); ++iter) {
		// 蛇头撞到了蛇身，即自己，或者边界。
		if ((snake.front().x == iter->x && snake.front().y == iter->y) || (snake.front().x < 0 || snake.front().x >= WIDTH / SIZE || snake.front().y < 0 || snake.front().y >= HEIGHT / SIZE)) {
			// 游戏结束

			// 设置字体颜色。
			settextcolor(RED);
			// 设置字体风格。
			// 第一个参数是高度，第二个参数如果是0的话即根据高度自适应，最后一个参数是字体。_T即将convert。
			settextstyle(100, 0, _T("Simsun"));
			TCHAR endString[16];
			_stprintf_s(endString, _T("笨蛋小郝！"));
			// 输出游戏结束文字。
			outtextxy(WIDTH / SIZE, HEIGHT / SIZE, endString);
			// 使游戏结束界面停留三秒中。
			Sleep(2000);
			closegraph();
			// 直接结束程序。
			exit(0);
		}
	}
}

// 直接将grow规定为false意味着如果我运行这个函数时，没有指定grow，则默认为false。
void moveSnake(std::deque<Snake>& snake, bool grow = false) {
	// 获取蛇头
	Snake newHead = snake.front();
	// 判断蛇的移动方向
	switch (newHead.dir) {
		case UP:
			newHead.y--;
			break;
		case DOWN:
			newHead.y++;
			break;
		case LEFT:
			newHead.x--;
			break;
		case RIGHT:
			newHead.x++;
			break;
	}
	// 将新的蛇头加到前面作为蛇头。
	snake.push_front(newHead);
	// 这个时候还需要去看看是否违规结束。
	checkEnd(snake);
	// 如果grow = true那么意味着我们吃到了食物，则不需要去删除最后一个蛇身，否则删除。
	if (!grow) {
		// 没有吃到食物,则清楚最后一个身体。
		// 这与为什么我们需要clear，因为当身体在途中生成的时候，不会消失，除非将其清除。
		clearrectangle(snake.back().x * SIZE, snake.back().y * SIZE, (snake.back().x + 1) * SIZE, (snake.back().y + 1) * SIZE);
		snake.pop_back();
	}
	drawSnake(snake);
}

// 更改蛇的移动方向。
void changeDirecton(std::deque<Snake>& snake) {
	ExMessage msg = { 0 };
	// 捕获键盘消息
	peekmessage(&msg, EX_KEY);
	// 当某个键被按下。
	if (msg.message == WM_KEYDOWN) {
		switch (msg.vkcode) {
			case 'W':
			case 'w':
			case VK_UP:
				if (snake.front().dir != DOWN) {
					snake.front().dir = UP;
				}
				break;
			case 'S':
			case 's':
			case VK_DOWN:
				if (snake.front().dir != UP) {
					snake.front().dir = DOWN;
				}
				break;
			case 'A':
			case 'a':
			case VK_LEFT:
				if (snake.front().dir != RIGHT) {
					snake.front().dir = LEFT;
				}
				break;
			case 'D':
			case 'd':
			case VK_RIGHT:
				if (snake.front().dir != LEFT) {
					snake.front().dir = RIGHT;
				}
				break;
		}
	}
}

void createFood(Food& food, std::deque<Snake>& snake) {
	while (true) {
		// 随机生成食物。
		food.x = rand() % (WIDTH / SIZE);
		food.y = rand() % (HEIGHT / SIZE);

		bool overlap = false;
		for (auto& body : snake) {
			if (food.x == body.x && food.y == body.y) {
				overlap = true;
				break;
			}
		}
		if (!overlap) {
			break;
		}
	}

}

bool checkCollision(std::deque<Snake>& snake, Food& food) {
	if (snake.front().x == food.x && snake.front().y == food.y) {
		return true;
	}
	return false;
}

// MAIN函数。
int main() {
	// 绘制窗口，两个参数发分别为窗口的宽度与高度。
	initgraph(WIDTH, HEIGHT);
	// 定义一条蛇
	std::deque<Snake> snake;
	// 设置得分信息。
	int score = 0;
	// 生成食物。
	Food food;
	createFood(food, snake);
	// 绘制食物/
	setfillcolor(RED);
	fillrectangle(food.x * SIZE, food.y * SIZE, (food.x + 1) * SIZE, (food.y + 1) * SIZE);

	// 初始化一个蛇身。
	snake.push_front({ WIDTH / SIZE / 2, HEIGHT / SIZE / 2, RIGHT });
	drawSnake(snake);

	while (true) {
		moveSnake(snake);
		changeDirecton(snake);
		// 显示得分信息。
		settextcolor(YELLOW);
		settextstyle(20, 0, _T("Consolas"));
		TCHAR strScore[16];
		_stprintf_s(strScore, _T("Score: %d"), score);
		outtextxy(10, 10, strScore);
		// 检查蛇是否吃到食物。
		if (checkCollision(snake, food)) {
			score++;
			// 重新生成食物
			createFood(food, snake);
			setfillcolor(RED);
			fillrectangle(food.x * SIZE, food.y * SIZE, (food.x + 1) * SIZE, (food.y + 1) * SIZE);
			moveSnake(snake, true);
		}
	}

	system("pause");

	// 记得每次不需要它的时候关闭窗口，防止内存泄漏。
	closegraph();
	return 0;
}