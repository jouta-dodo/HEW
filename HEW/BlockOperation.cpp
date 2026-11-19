#include "BlockOperation.h"
#include "Input.h"
#include <ctime>

void BlockOperation::Init()
{
	srand((unsigned)time(NULL));

	blockdata.LoadBlockCSV("asset/stage1.csv");

	//m_player.Init("asset/char01.png", 3, 4);
	//m_player.SetPos(0.0f, 0.0f, 0.0f);
	//m_player.SetSize(200.0f, 300.0f, 0.0f);
	//m_player.SetAngle(0.0f);
	//m_player.SetColor(1.0f, 1.0f, 1.0f, 1.5f);

	////m_background.Init("asset/back_img_01.png");
	//m_background.Init("asset/mblock.png");
	//m_background.SetPos(0.0f, 0.0f, 0.0f);
	//m_background.SetSize(720.0f, 640.0f, 0.0f);//640/480
	//m_background.SetAngle(0.0f);

	//for (int x = 0;x < upperWidth;x++)
	//{
	//	for (int y = 0;y < upperHeight;y++)
	//	{
	//		const float upperPosX = upperBlockSize * (x - upperWidth / 2);
	//		const float upperPosY = -upperBlockSize * (y - upperHeight / 2) + 100;

	//		InitBlocks(m_upperMovingblock[x][y], upperPosX, upperPosY, upperBlockSize);
	//		InitBlocks(m_upperFixingblock[x][y], upperPosX, upperPosY, upperBlockSize);
	//		InitBlocks(m_upperSelectingblock[x][y], upperPosX, upperPosY, upperBlockSize);
	//	}
	//}

	//for (int x = 0;x < lowerWidth;x++)
	//{
	//	for (int y = 0;y < lowerHeight;y++)
	//	{
	//		const float lowerPosX = lowerBlockSize * (x - lowerWidth / 2);
	//		const float lowerPosY = -lowerBlockSize * (y - lowerHeight / 2) - 100;

	//		InitBlocks(m_lowerSelectingblock[x][y], lowerPosX, lowerPosY, lowerBlockSize);
	//		InitBlocks(m_lowerNonselectingblocks[x][y], lowerPosX, lowerPosY, lowerBlockSize);

	//		m_lowerdata[x][y][lowerUnselected] = blockdata.GetBlockData(y, x);
	//	}
	//}

	for (int x = 0;x < upperWidth;x++)
	{
		for (int y = 0;y < upperHeight;y++)
		{
			m_upperblocks[x][y].type = BlockType::none;
			//m_upperblocks[x][y].state = BlockState::Enpty;
		}
	}

	for (int x = 0;x < lowerWidth;x++)
	{
		for (int y = 0;y < lowerHeight;y++)
		{
			m_lowerblocks[x][y].type = BlockType::none;
			//m_lowerblocks[x][y].state = BlockState::Unselected;
			m_lowerdata[x][y][lowerUnselected] = blockdata.GetBlockData(y, x);
		}
	}

}

//void BlockOperation::InitBlocks(Object& block, float posx, float posy, int size)
//{
//	block.Init("asset/block.png");
//	//block.Init("asset/siroiro.png");
//	//block.Init("asset/whiteblock.png");
//	block.SetPos(posx, posy, 0.0f);
//	block.SetSize(size, size, 0.0f);
//}

void BlockOperation::Update()
{
	// 入力の更新
	//m_input.Update();
	// 
	//色の設定
	//SetColor();

	if ((GetAsyncKeyState('R') & 0x8000)) 
	{
		ResetBlocks();
	}

	switch (currentPhase)
	{
	case Phase::LowerBlockSelect:
		
		//どの位置のブロックを選択するか
		lowerSelectBlock();

		//SelectBlockで選ばれたブロックの左上の座標に計算
		startX = (blockNum % 3) * 4;
		startY = (blockNum / 3) * 4;

		//一旦選ばれてるブロックを表示する配列を０に入れる
		for (int x = 0;x < lowerWidth;x++)
		{
			for (int y = 0;y < lowerHeight;y++)
			{
				m_lowerdata[x][y][lowerSelected] = 0;
				//m_lowerblocks[x][y].state = BlockState::Enpty;
			}
		}

		for (int x = 0;x < blocklistWidth;x++)
		{
			for (int y = 0;y < blocklistHeight;y++)
			{
				//選ばれてるブロックを表示する配列に入れる
				m_lowerdata[startX + x][startY + y][lowerSelected] = m_lowerdata[startX + x][startY + y][lowerUnselected];
				blockList[y][x] = m_lowerdata[startX + x][startY + y][lowerSelected];
				//m_lowerblocks[startX + x][startY + y].state = BlockState::Selected;
			}
		}
		if (Input::IsKeyPressed(Key::Enter))
		{
			//選ばれたブロックを上に出す
			for (int x = 0;x < blocklistWidth;x++)
			{
				for (int y = 0;y < blocklistHeight;y++)
				{
					if (blockList[x][y] != 0)
					{
						PutBlock();
						currentPhase = Phase::UpperDecideBlockPlace;
					}
				}
			}
		}

		//Block消す
		for (int i = 0;i < MAXBLOCK;i++)
		{
			if (SetedBlocknum[i] != 0)
			{
				num = SetedBlocknum[i] - 1;
				deleteX = (num % 3) * 4;
				deleteY = (num / 3) * 4;
				for (int x = 0;x < blocklistWidth;x++)
				{
					for (int y = 0;y < blocklistHeight;y++)
					{
						m_lowerdata[deleteX + x][deleteY + y][lowerUnselected] = 0;
						//m_lowerblocks[x][y].state = BlockState::Enpty;
					}
				}
			}
		}

		break;
	case Phase::UpperDecideBlockPlace:
		//上の配列の中でブロックを動かす
		moveBlock();

		if (Input::IsKeyPressed(Key::Enter))
		{
			//ngFgを返り値にしてるから反転してる
			isSet = !SetBlock();

			//置けたら下のブロック選択画面へ
			if (isSet == true)
			{
				for (int i = 0;i < MAXBLOCK;i++)
				{
					if (SetedBlocknum[i] == 0)
					{
						SetedBlocknum[i] = blockNum + 1;
						break;
					}
				}
				//isMove = false;
				currentPhase = Phase::LowerBlockSelect;
			}
		}
		break;
	case Phase::UpperBlockSelect:
		if (isMove == false)
		{
			m_upperdata[0][0][upperSelecting] = 1;
			isMove = true;
		}

		upperSelectBlock();

		if (Input::IsKeyPressed(Key::Enter))
		{
			for (int x = 0;x < upperWidth;x++)
			{
				for (int y = 0;y < upperHeight;y++)
				{
					//選ばれたブロックのマスを見て０以外が保存されてたらそのブロックの種類を保存する
					if (m_upperdata[x][y][upperSelecting] != 0 &&
						m_upperdata[x][y][upperRecording] != 0)
					{
						RecordBlock = m_upperdata[x][y][upperRecording];
						m_upperdata[x][y][upperSelecting] = 0;
						break;
					}
				}
				if (RecordBlock != 0)break;
			}

			//ブロックの種類が保存されてたら上の画面で動かせるようにする
			if (RecordBlock != 0)
			{
				for (int x = 0;x < upperWidth;x++)
				{
					for (int y = 0;y < upperHeight;y++)
					{
						if (m_upperdata[x][y][upperRecording] == RecordBlock)
						{
							m_upperdata[x][y][upperMoving] = m_upperdata[x][y][upperFixing];
							m_upperdata[x][y][upperFixing] = 0;
							m_upperdata[x][y][upperRecording] = 0;
						}
					}
				}

				//上に設置したときに消した下のブロックを元に戻す
				blockNum = RecordBlock - 1;
				restoreX = (blockNum % 3) * 4;
				restoreY = (blockNum / 3) * 4;
				for (int x = 0;x < blocklistWidth;x++)
				{
					for (int y = 0;y < blocklistHeight;y++)
					{
						m_lowerdata[restoreX + x][restoreY + y][lowerUnselected] = blockdata.GetBlockData(restoreY + y, restoreX + x);
						m_lowerdata[restoreX + x][restoreY + y][lowerSelected] = blockdata.GetBlockData(restoreY + y, restoreX + x);
					}
				}
				//設置されてないことにする
				for (int i = 0;i < MAXBLOCK;i++)
				{
					if (SetedBlocknum[i] == RecordBlock)
					{
						SetedBlocknum[i] = 0;
						break;
					}
				}

				RecordBlock = 0;
				isMove = false;
				currentPhase = Phase::UpperDecideBlockPlace;
			}
		}
		break;
	default:
		break;
	}

}

void BlockOperation::Draw()
{
	// ゲーム処理実行
	//RendererDrawStart();

	//m_background.Draw();
	////m_player.Draw();
	//for (int x = 0;x < upperWidth;x++)
	//{
	//	for (int y = 0;y < upperHeight;y++)
	//	{
	//		m_upperFixingblock[x][y].Draw();
	//		m_upperMovingblock[x][y].Draw();
	//		m_upperSelectingblock[x][y].Draw();
	//	}
	//}

	//for (int x = 0;x < lowerWidth;x++)
	//{
	//	for (int y = 0;y < lowerHeight;y++)
	//	{
	//		m_lowerNonselectingblocks[x][y].Draw();
	//		m_lowerSelectingblock[x][y].Draw();
	//	}
	//}

	//RendererDrawEnd();
}

void BlockOperation::Uninit()
{
	/*m_background.Uninit();
	m_player.Uninit();
	for (int x = 0;x < upperWidth;x++)
	{
		for (int y = 0;y < upperHeight;y++)
		{
			m_upperMovingblock[x][y].Uninit();
			m_upperFixingblock[x][y].Uninit();
			m_upperSelectingblock[x][y].Uninit();
			m_lowerSelectingblock[x][y].Uninit();
			m_lowerNonselectingblocks[x][y].Uninit();
		}
	}
	RendererUninit();*/
}

void BlockOperation::upperSelectBlock()
{
	if (Input::IsKeyPressed(Key::A))
	{
		upperSelectMove(-1, 0);
	}

	if (Input::IsKeyPressed(Key::D))
	{
		upperSelectMove(1, 0);
	}

	if (Input::IsKeyPressed(Key::W))
	{
		upperSelectMove(0, -1);
	}

	if (Input::IsKeyPressed(Key::S))
	{
		upperSelectMove(0, 1);
	}
}

void BlockOperation::upperSelectMove(int x, int y)
{
	int ngFg = IsInvalidBlockMove(x, y, upperSelecting);

	if (ngFg == 0)
	{
		ShiftBlock(x, y, upperSelecting);
	}
	if (ngFg == 2)
	{
		for (int x = 0;x < upperWidth;x++)
		{
			for (int y = 0;y < upperHeight;y++)
			{
				m_upperdata[x][y][upperSelecting] = {};
			}
		}
		isMove = false;
		currentPhase = Phase::LowerBlockSelect;
	}
}

void BlockOperation::lowerSelectBlock()
{
	if (Input::IsKeyPressed(Key::A))
	{
		lowerMove(-1, 0);
	}

	if (Input::IsKeyPressed(Key::D))
	{
		lowerMove(1, 0);
	}

	if (Input::IsKeyPressed(Key::W))
	{
		lowerMove(0, 1);
	}

	if (Input::IsKeyPressed(Key::S))
	{
		lowerMove(0, -1);
	}
}

void BlockOperation::lowerMove(int x, int y)
{
	if (blockNum < 3)
	{
		blockNum += x;
		if (blockNum < 0)
		{
			blockNum = 2;
		}

		if (blockNum > 2)
		{
			blockNum = 0;
		}

		if (y == 1)
		{
			currentPhase = Phase::UpperBlockSelect;
		}
		if (y == -1)
		{
			blockNum += 3;
		}
	}
	else
	{
		blockNum += x;
		if (blockNum < 3)
		{
			blockNum = 5;
		}

		if (blockNum > 5)
		{
			blockNum = 3;
		}

		if (y != 0)
		{
			blockNum -= 3;
		}
	}
}

void BlockOperation::PutBlock()
{
	for (int x = 0;x < 3;x++)
	{
		for (int y = 0;y < 3;y++)
		{
			//m_upperblocks[x][y].state = BlockState::Moving;
			m_upperdata[x][y][upperMoving] = blockList[y][x];
		}
	}
}

void BlockOperation::ShiftBlock(int dirx, int diry, int state)
{
	int tmp[upperWidth][upperHeight][upperBlockState] = {};

	for (int x = 0;x < upperWidth;x++)
	{
		for (int y = 0;y < upperHeight;y++)
		{
			if (m_upperdata[x][y][state] != 0)
			{
				tmp[x + dirx][y + diry][state] = m_upperdata[x][y][state];
			}
		}
	}
	for (int x = 0;x < upperWidth;x++)
	{
		for (int y = 0;y < upperHeight;y++)
		{
			m_upperdata[x][y][state] = tmp[x][y][state];
		}
	}
}

void BlockOperation::moveBlock()
{
	if (Input::IsKeyPressed(Key::A))
	{
		moveBlockX(-1);
	}

	if (Input::IsKeyPressed(Key::D))
	{
		moveBlockX(1);
	}

	if (Input::IsKeyPressed(Key::W))
	{
		moveBlockY(-1);
	}

	if (Input::IsKeyPressed(Key::S))
	{
		moveBlockY(1);
	}
}

void BlockOperation::moveBlockX(int x)
{
	bool ngFg = IsInvalidBlockMove(x, 0, upperMoving);

	if (ngFg == false)
	{
		ShiftBlock(x, 0, upperMoving);
	}
	else
	{
		OutofBlock();
	}
}

void BlockOperation::moveBlockY(int y)
{
	bool ngFg = IsInvalidBlockMove(0, y, upperMoving);

	if (ngFg == false)
	{
		ShiftBlock(0, y, upperMoving);
	}
	else
	{
		OutofBlock();
	}
}

void BlockOperation::OutofBlock()
{
	for (int x = 0;x < upperWidth;x++)
	{
		for (int y = 0;y < upperHeight;y++)
		{
			m_upperdata[x][y][upperMoving] = 0;
		}
	}
	currentPhase = Phase::LowerBlockSelect;
}

int BlockOperation::IsInvalidBlockMove(int dirx, int diry, int state)
{
	int ngFg = 0;
	switch (state)
	{
	case upperMoving:
		for (int x = 0;x < upperWidth;x++)
		{
			for (int y = 0;y < upperHeight;y++)
			{
				if (m_upperdata[x][y][state] != 0 &&
					(x + dirx < 0 ||
						x + dirx >= upperWidth ||
						y + diry < 0 ||
						y + diry >= upperHeight))
				{
					ngFg = true;
					break;
				}
			}
			if (ngFg == true)
			{
				break;
			}
		}
		return ngFg;
		break;

	case upperSelecting:
		for (int x = 0;x < upperWidth;x++)
		{
			for (int y = 0;y < upperHeight;y++)
			{
				if (m_upperdata[x][y][state] == 1 &&
					(x + dirx < 0 ||
						x + dirx >= upperWidth ||
						y + diry < 0))
				{
					ngFg = 1;
					break;
				}
				else if (m_upperdata[x][y][state] != 0 &&
					y + diry >= upperHeight)
				{
					ngFg = 2;
					break;
				}
			}
			if (ngFg != 0)
			{
				break;
			}
		}
		return ngFg;
		break;
	default:
		break;
	}

}


bool BlockOperation::SetBlock()
{
	bool ngFg = IsInvalidBlockSet();

	if (ngFg == false)
	{
		for (int x = 0;x < upperWidth;x++)
		{
			for (int y = 0;y < upperHeight;y++)
			{
				if (m_upperdata[x][y][upperMoving] != 0)
				{
					//m_upperblocks[x][y].state = BlockState::Fixing;
					m_upperdata[x][y][upperFixing] = m_upperdata[x][y][upperMoving];
					m_upperdata[x][y][upperRecording] = blockNum + 1;
					m_upperdata[x][y][upperMoving] = 0;
				}
			}
		}
	}
	return ngFg;
}

bool BlockOperation::IsInvalidBlockSet()
{
	bool ngFg = false;

	for (int x = 0;x < upperWidth;x++)
	{
		for (int y = 0;y < upperHeight;y++)
		{
			//moveの0以外のマスとfixedの0以外のマスが重なってたらダメ
			if (m_upperdata[x][y][upperFixing] != 0 &&
				m_upperdata[x][y][upperMoving] != 0)
			{
				ngFg = true;
				break;
			}
		}
		if (ngFg == true)
		{
			break;
		}
	}
	return ngFg;
}

void BlockOperation::ResetBlocks()
{
	for (int x = 0;x < upperWidth;x++)
	{
		for (int y = 0;y < upperHeight;y++)
		{
			for (int i = 0;i < upperBlockState;i++)
			{
				m_upperdata[x][y][i] = {};
				//m_upperblocks[x][y].state = BlockState::Enpty;
			}
		}
	}
	for (int x = 0;x < lowerWidth;x++)
	{
		for (int y = 0;y < lowerHeight;y++)
		{
			m_lowerdata[x][y][lowerUnselected] = blockdata.GetBlockData(y, x);
			//m_lowerblocks[x][y].state = BlockState::Unselected;
		}
	}
	for (int i = 0;i < MAXBLOCK;i++)
	{
		SetedBlocknum[i] = {};
	}
	blockNum = 0;
}

void BlockOperation::SetColor()
{
	for (int x = 0;x < upperWidth;x++)
	{
		for (int y = 0;y < upperHeight;y++)
		{
			color = static_cast<Color>(m_upperdata[x][y][upperFixing]);
			switch (color)
			{
			case Color::none:		//空白
				m_upperblocks[x][y].type = BlockType::none;
				break;
			case Color::red:		//赤色
				m_upperblocks[x][y].type = BlockType::red;
				break;
			case Color::blue:		//青色
				m_upperblocks[x][y].type = BlockType::blue;
				break;
			case Color::yellow:		//黄色
				m_upperblocks[x][y].type = BlockType::yellow;
				break;
			}
		}
	}
	for (int x = 0;x < upperWidth;x++)
	{
		for (int y = 0;y < upperHeight;y++)
		{
			color = static_cast<Color>(m_upperdata[x][y][upperMoving] + 3);
			switch (color)
			{
			case Color::none:		//空白
				m_upperblocks[x][y].type = BlockType::none;
				break;
			case Color::rightred:	//薄い赤色
				m_upperblocks[x][y].type = BlockType::rightred;
				break;
			case Color::rightblue:	//薄い青色
				m_upperblocks[x][y].type = BlockType::rightblue;
				break;
			case Color::rightyellow://薄い黄色
				m_upperblocks[x][y].type = BlockType::rightyellow;
				break;
			default:
				m_upperblocks[x][y].type = BlockType::none;
				break;
			}
		}
	}

	for (int x = 0;x < upperWidth;x++)
	{
		for (int y = 0;y < upperHeight;y++)
		{
			color = static_cast<Color>(m_upperdata[x][y][upperSelecting] + 6);
			switch (color)
			{
			case Color::none:		//空白
				m_upperblocks[x][y].type = BlockType::none;
				break;
			case Color::black:		//黒色
				m_upperblocks[x][y].type = BlockType::black;
				break;
			default:
				m_upperblocks[x][y].type = BlockType::none;
				break;
			}
		}
	}

	for (int x = 0;x < lowerWidth;x++)
	{
		for (int y = 0;y < lowerHeight;y++)
		{
			color = static_cast<Color>(m_lowerdata[x][y][lowerSelected]);
			switch (color)
			{
			case Color::none:		//空白
				m_lowerblocks[x][y].type = BlockType::none;
				break;
			case Color::red:		//赤色
				m_lowerblocks[x][y].type = BlockType::red;
				break;
			case Color::blue:		//青色
				m_lowerblocks[x][y].type = BlockType::blue;
				break;
			case Color::yellow:		//黄色
				m_lowerblocks[x][y].type = BlockType::yellow;
				break;
			default:
				break;
			}
		}
	}
	for (int x = 0;x < lowerWidth;x++)
	{
		for (int y = 0;y < lowerHeight;y++)
		{
			color = static_cast<Color>(m_lowerdata[x][y][lowerUnselected] + 3);
			switch (color)
			{
			case Color::none:		//空白
				m_lowerblocks[x][y].type = BlockType::none;
				break;
			case Color::rightred:	//薄い赤色
				m_lowerblocks[x][y].type = BlockType::rightred;
				break;
			case Color::rightblue:	//薄い青色
				m_lowerblocks[x][y].type = BlockType::rightblue;
				break;
			case Color::rightyellow://薄い黄色
				m_lowerblocks[x][y].type = BlockType::rightyellow;
				break;
			default:
				m_lowerblocks[x][y].type = BlockType::none;
				break;
			}
		}
	}
}



int BlockOperation::GetCurrentPhase()
{
	switch (currentPhase)
	{
	case Phase::LowerBlockSelect:
		return 0;
		break;
	case Phase::UpperDecideBlockPlace:
		return 1;
		break;
	case Phase::UpperBlockSelect:
		return 2;
		break;
	default:
		break;
	}
	return 0;
}

int BlockOperation::GetUpperBlockData(int x, int y)
{
	return m_upperdata[x][y][upperFixing];
}

std::array<std::array<int, upperHeight>, upperWidth> BlockOperation::GetUpperBlockData()
{
	std::array<std::array<int, upperHeight>, upperWidth> tmp = {};

	for (int x = 0;x < upperWidth;x++)
	{
		for (int y = 0;y < upperHeight ; y++)
		{
			tmp[x][y] = m_upperdata[x][y][upperFixing];
		}
	}

	return tmp;
}