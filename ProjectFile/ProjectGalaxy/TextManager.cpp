#include "TextManager.h"
#include"FontManager.h"
#include"SoundManager.h"
#include<tchar.h>

namespace
{
	constexpr int kFontSize = 20;
    constexpr int kTextHeightDistance = 5;

    const char* InputTextSEName = "InputText.mp3";
}

TextManager::TextManager()
	:m_fontHandle(FontManager::GetInstance().GetFontData("SF_font.ttf","廻想体 ネクスト UP B",20)),
	m_drawTextLength(0),
	m_drawTextFrame(0),
    m_inputTextSEHandle(SoundManager::GetInstance().GetSoundData(InputTextSEName)),
    m_postFrameTextSize(0)
{
    ChangeVolumeSoundMem(100, m_inputTextSEHandle);

}

TextManager::~TextManager()
{
}

void TextManager::Init()
{
}

void TextManager::Update()
{
}

void TextManager::Draw()
{
    int textBoxTextsLength = 0;

    // 全文字数を計算
    for (auto& texts : m_texts.front())
    {
        textBoxTextsLength += texts.length();
    }

    m_drawTextFrame++;

    // 単文字描画のインターバル
    if (m_drawTextFrame > 0)
    {
        m_drawTextFrame = 0;
        // テキストボックスの文字列サイズ以上にならないようにする
        if (m_drawTextLength < textBoxTextsLength)
        {
            m_drawTextLength++;
        }
    }

    // 描画するテキストボックス内の今の行の文字総数
    auto textBoxIndex = m_texts.front().begin();

    std::list<std::string> drawText;
    drawText.resize(m_texts.front().size());

    int remainingLength = m_drawTextLength; // 描画可能な文字数

    for (auto text = drawText.begin(); text != drawText.end();)
    {
        // 現在の行のテキストを処理
        if (textBoxIndex != m_texts.front().end())
        {
            int charsToAdd = min((int)textBoxIndex->size(), remainingLength); // 現行の残り文字数を計算
            *text += textBoxIndex->substr(0, charsToAdd); // 必要な分だけ文字を追加
            remainingLength -= charsToAdd;

            // 全部処理したら次の行に進む
            if (charsToAdd >= (int)textBoxIndex->size())
            {
                textBoxIndex++;
            }
        }

        text++;
    }

    textBoxIndex = m_texts.front().begin();
    int listIndex = 0;

    // テキストを描画
    for (auto text = drawText.begin(); text != drawText.end();)
    {
        DrawFormatStringFToHandle(300, 200 + listIndex * (kFontSize+ kTextHeightDistance), 0xffffff, m_fontHandle, text->c_str());
        listIndex++;
        text++;
    }

    //前のフレームより表示するテキストの数が増えていたらテキストが入力された
    if (m_postFrameTextSize < m_drawTextLength)
    {
        //SEを流す
        PlaySoundMem(m_inputTextSEHandle, DX_PLAYTYPE_BACK);
    }
    m_postFrameTextSize = m_drawTextLength;
   
	
}

void TextManager::InText(const std::string text)
{
	std::list<std::string> strings;
	strings.push_back(text);
	m_texts.push_back(strings);
}

void TextManager::InTexts(const std::list<std::string> text)
{
	m_texts.push_back(text);
}

void TextManager::DeleteText()
{
	m_drawTextLength = 0;
    m_postFrameTextSize = 0;
	m_texts.pop_front();
}



