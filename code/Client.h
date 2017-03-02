

class IClient
{
public:
	virtual void OnCreate() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnDestroy() = 0;
};


extern IClient* MainClient;