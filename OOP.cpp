
class Building
{
    int floors;
    int rooms;
    const char* address;
    float selling_price;

    bool living()
    {
        return false;
    }

    void purchase()
    {

    }

    void sell()
    {

    }

};


int main()
{
    Building obj;

    obj.living();
}