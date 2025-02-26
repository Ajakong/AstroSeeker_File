#include "TextManager.h"
#include"FontManager.h"
#include"SoundManager.h"
#include"Game.h"
#include<tchar.h>

namespace
{
	constexpr int kFontSize = 28;
    constexpr int kTextHeightDistance = 16;
    
    constexpr float kTextStartPos = 200.f;
    constexpr float kExrate = 0.25f;


    const char* InputTextSEName = "InputText.mp3";


}

TextManager::TextManager()
	:m_fontHandle(FontManager::GetInstance().GetFontData("SF_font.ttf","廻想体 ネクスト UP B",kFontSize)),
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

    // 最前テキストボックスの全文字数を計算 m_texts.front:最前面のテキストボックス texts:その行
    for (auto& texts : m_texts.front())
    {
        //テキストボックスの行ごとの文字数を追加
        textBoxTextsLength += static_cast<int>(texts.length());
    }

    //一文字描画するときのインターバル
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
        DrawFormatStringFToHandle(
            static_cast<float>(Game::kScreenWidth/2)-Game::kScreenWidth*kExrate,
            static_cast<float>(kTextStartPos + listIndex * (kFontSize+ kTextHeightDistance)),
            0xffffff,
            m_fontHandle,
            text->c_str()
        );
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

void TextManager::InNextText(const std::string text)
{
    std::list<std::string> string;
    string.push_back(text);
    std::list<std::list<std::string>> strings;
    strings.push_back(string);
    m_nextTexts.push_back(strings);
}

void TextManager::InTexts(const std::list<std::string> text)
{
	m_texts.push_back(text);
}

void TextManager::InNextTexts(const std::list<std::string> text)
{
    std::list<std::list<std::string>> strings;
    strings.push_back(text);

    m_nextTexts.push_back(strings);
}

void TextManager::SetTexts(const std::list<std::list<std::string>> texts)
{
    m_texts = texts;
}
void TextManager::SetNextTexts(const std::list<std::list<std::string>> texts)
{
    m_nextTexts.push_back(texts);
}
void TextManager::DeleteText()
{
	m_drawTextLength = 0;
    m_postFrameTextSize = 0;
	m_texts.pop_front();
   
}



