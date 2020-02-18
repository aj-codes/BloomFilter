// Filter.h
//

// Simple interface for a filter that can add data, or check if the data already exists
class IFilter
{
public:
    IFilter() {}
    virtual ~IFilter() {}
    
    virtual void Add(const wchar_t* elementData) = 0;
    virtual bool Check(const wchar_t* elementData) = 0;
};
