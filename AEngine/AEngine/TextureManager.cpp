#include"TextureManager.h"

TextureManager::TextureManager(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext)
{
    m_pD3DDevice = pD3DDevice;
    m_pImmediateContext = pImmediateContext;
    m_pTextureCollection = new std::vector<TextureObject>;
}

TextureManager::~TextureManager()
{
    
}

Task* TextureManager::ImportTextures()
{
    Task* task = new Task;
    TaskData* data = new TaskData;
    data->parameter1 = (void*)m_pTextureCollection;
    data->parameter2 = nullptr;
    data->parameter3 = nullptr;
    data->parameter4 = nullptr;
    data->parameter5 = nullptr;

    task->thisPointer = this;
    task->pTaskData = data;
    task->callback = DoneImporting;
    task->taskFunction = DoImportTextureTask;
    return task; 
}

void TextureManager::SetTexture(unsigned int ID)
{
    m_pImmediateContext->PSSetShaderResources(0,1,&m_pTextureCollection->at(ID).texture);
}

void TextureManager::DoImportTask(TaskData* data)
{
    std::vector<TextureObject>* m_pTextures = static_cast<std::vector<TextureObject>*>(data->parameter1);
    HRESULT hr;
    TextureObject temp;

    hr = D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
        L"D:\\Projects\\Ares\\AresEngine\\AEngine\\Debug\\Content\\head.dds", NULL,NULL,&temp.texture,NULL);
    assert(!FAILED(hr));
    temp.id = 0;
    m_pTextures->push_back(temp);

    hr = D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
        L"D:\\Projects\\Ares\\AresEngine\\AEngine\\Debug\\Content\\jacket.dds", NULL,NULL,&temp.texture,NULL);
    assert(!FAILED(hr));

    assert(!FAILED(hr));
    temp.id = 1;
    m_pTextures->push_back(temp);

    hr = D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
        L"D:\\Projects\\Ares\\AresEngine\\AEngine\\Debug\\Content\\pants.dds", NULL,NULL,&temp.texture,NULL);
    assert(!FAILED(hr));

    assert(!FAILED(hr));
    temp.id = 2;
    m_pTextures->push_back(temp);

    hr = D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
        L"D:\\Projects\\Ares\\AresEngine\\AEngine\\Debug\\Content\\upBodyC.dds", NULL,NULL,&temp.texture,NULL);
    assert(!FAILED(hr));

    assert(!FAILED(hr));
    temp.id = 3;
    m_pTextures->push_back(temp);
}

void TextureManager::DoneImportingTask(void* task)
{
    //need to implement an 
    //better option like an event or message 
    //instead of setting an bool flag
    m_Finished = true;
}


