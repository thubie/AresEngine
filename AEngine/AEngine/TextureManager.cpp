#include"TextureManager.h"

TextureManager::TextureManager(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext,AEngine* pEngine )
{
    m_pD3DDevice = pD3DDevice;
    m_pD3DDevice->AddRef();

    m_pImmediateContext = pImmediateContext;
    m_pImmediateContext->AddRef();
    m_pEngine = pEngine; 
    m_pTextureCollection = new std::vector<TextureObject>;
}

TextureManager::~TextureManager()
{
    CleanUpResources();
    
    if(m_pD3DDevice != nullptr)
    {
        m_pD3DDevice->Release();
        m_pD3DDevice = nullptr;
    }

    if(m_pImmediateContext != nullptr)
    {
        m_pImmediateContext->Release();
        m_pImmediateContext = nullptr;
    }
}

void TextureManager::DoImportTextureTask(TaskData* pData, void* thisPointer)
{
    TextureManager* self = static_cast<TextureManager*>(thisPointer);
    self->DoImportTask(pData);
}

void TextureManager::DoneImporting(void* thispointer, void* task)
{
    TextureManager*  self = static_cast<TextureManager*>(thispointer);
    self->DoneImportingTask(task);
}

void TextureManager::CleanUpResources()
{
    for(int i = 0; i < m_pTextureCollection->size(); ++i)
    {
        m_pTextureCollection->at(i).texture->Release();
    }
}

Task* TextureManager::ImportTextures(const char* texturesPath)
{
    Task* task = new Task;
    TaskData* data = new TaskData;
    data->parameter1 = (void*)m_pTextureCollection;
    data->parameter2 = (void*)texturesPath;
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
    const char* texturePath = (const char*)data->parameter2;
    
    HRESULT hr;
    TextureObject temp;
    
    const unsigned int length = 1024;
    size_t convertedChar;

    char texture1[length];
    char texture2[length];
    char texture3[length];
    char texture4[length];
    WCHAR CurrentTexturePath[length];
    
    strcpy_s(texture1, texturePath);
    strcpy_s(texture2, texturePath);
    strcpy_s(texture3, texturePath);
    strcpy_s(texture4, texturePath);

    strcat_s(texture1, "\\head.dds");
    strcat_s(texture2, "\\jacket.dds");
    strcat_s(texture3, "\\pants.dds");
    strcat_s(texture4, "\\upBodyC.dds");

    mbstowcs_s(&convertedChar,CurrentTexturePath,texture1,length);
    hr = D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
        CurrentTexturePath, NULL,NULL,&temp.texture,NULL);
    assert(!FAILED(hr));
    temp.id = 0;
    m_pTextures->push_back(temp);

    mbstowcs_s(&convertedChar,CurrentTexturePath,texture2,length);
    hr = D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
        CurrentTexturePath, NULL,NULL,&temp.texture,NULL);
    assert(!FAILED(hr));
    temp.id = 1;
    m_pTextures->push_back(temp);

    mbstowcs_s(&convertedChar,CurrentTexturePath,texture3,length);
    hr = D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
        CurrentTexturePath, NULL,NULL,&temp.texture,NULL);
    assert(!FAILED(hr));
    temp.id = 2;
    m_pTextures->push_back(temp);

    mbstowcs_s(&convertedChar,CurrentTexturePath,texture4,length);
    hr = D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
       CurrentTexturePath, NULL,NULL,&temp.texture,NULL);
    assert(!FAILED(hr));
    temp.id = 3;
    m_pTextures->push_back(temp);
}

void TextureManager::DoneImportingTask(void* task)
{
    Message message;
    message.MessageType =  IMPORT_TEXTURE_DONE;
    m_pEngine->SubmitMessage(message);
    m_Finished = true;
    delete task;
}


