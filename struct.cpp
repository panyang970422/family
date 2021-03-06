#include "struct.hpp"

familyTree::familyTree()
{
    oldest = nullptr;
    currentPerson = nullptr;
}
familyTree::~familyTree()
{
    clear(oldest);
}
familyTree::familyTree(const familyTree &other)
{
    if (other.oldest == nullptr)
        oldest = nullptr;
    else
    {
        oldest = new Person(other.oldest->name, other.oldest->partnerName,
                            other.oldest->born_date, other.oldest->dead_date, other.oldest->isMale);
        copyTree(other.oldest, oldest);
        currentPerson = findName(other.getCurrentName(), oldest);
    }
}

bool familyTree::addPerson(bool isCompeer, string name_, string partnerName_,
                           string born, string dead, bool isMale_, Person *ptr_)
{
    if (findName(name_, oldest) != nullptr)
        return false;
    auto ptr = ptr_;
    if (ptr == nullptr && isCompeer && oldest == nullptr)
    {
        oldest = new Person(name_, partnerName_, born, dead, isMale_);
        currentPerson = oldest;
    }
    else if (ptr != nullptr && isCompeer)
    {
        auto guy = ptr;
        if (Date(born) < guy->born_date)
        {
            while((findPreCompeer(guy)!=nullptr) && (findPreCompeer(guy)->born_date > Date(born)))
            {
                guy = findPreCompeer(guy);
            }
            if (guy == oldest)
            {
                auto temp = new Person(name_, partnerName_, born, dead, isMale_);
                temp->compeer = oldest;
                oldest = temp;
            }
            else if (findPreCompeer(guy) == nullptr) {
                auto guy_parent = findParent(guy, oldest);
                guy_parent->junior = nullptr;
                guy_parent->junior = new Person(name_, partnerName_, born, dead, isMale_);
                guy_parent->junior->compeer = guy;
            }
            else {
                auto guy_brother = findPreCompeer(guy);
                guy_brother->compeer = nullptr;
                guy_brother->compeer = new Person(name_, partnerName_, born, dead, isMale_);
                guy_brother->compeer->compeer = guy;
            }
        }
        else
        {
            auto temp = new Person(name_, partnerName_, born, dead, isMale_);
            if (guy->compeer == nullptr)
            {
                guy->compeer = temp;
                return true;
            }
            while (Date(born) >= guy->compeer->born_date)
            {
                guy = guy->compeer;
                if (guy->compeer == nullptr)
                {
                    guy->compeer = temp;
                    return true;
                }
            }
            temp->compeer = guy->compeer;
            guy->compeer = temp;
        }
    }
    else if (ptr != nullptr && !isCompeer)
    {
        if (ptr->junior == nullptr)
        {
            ptr->junior = new Person(name_, partnerName_, born, dead, isMale_);
        }
        else
        {
            auto guy = ptr->junior;
            if (Date(born) < guy->born_date)
            {
                ptr->junior = nullptr;
                ptr->junior = new Person(name_, partnerName_, born, dead, isMale_);
                ptr->junior->compeer = guy;
            }
            else
            {
                auto temp = new Person(name_, partnerName_, born, dead, isMale_);
                if (guy->compeer == nullptr)
                {
                    guy->compeer = temp;
                    return true;
                }
                while (Date(born) >= guy->compeer->born_date)
                {
                    guy = guy->compeer;
                    if (guy->compeer == nullptr)
                    {
                        guy->compeer = temp;
                        return true;
                    }
                }
                temp->compeer = guy->compeer;
                guy->compeer = temp;
            }
        }
    }
    return true;
}
bool familyTree::addCompeer(string name, string partnerName, string born, string dead, bool isMale)
{
    return addPerson(true, name, partnerName, born, dead, isMale, currentPerson);
}
bool familyTree::addJunior(string name, string partnerName, string born, string dead, bool isMale)
{
    return addPerson(false, name, partnerName, born, dead, isMale, currentPerson);
}
void familyTree::print()
{
    if (oldest == NULL)
    {
        return;
    }
    traverse(oldest);
}
void familyTree::printCompeer()
{
    if (oldest == NULL)
    {
        return;
    }
    traverseCompeer(oldest);
}
void familyTree::printAllJunior()
{
    if (oldest == NULL)
    {
        return;
    }
    traverseCompeer(oldest->junior);
}
void familyTree::printByName(string name_)
{
    auto ptr = findName(name_, oldest);
    if (ptr == NULL)
        return;
    cout << "Name:" << ptr->name << " Wife:" << ptr->partnerName << endl;
}
void familyTree::printParentByName(string name_)
{
    auto ptr = findParent(findName(name_, oldest), oldest);
    if (ptr == NULL)
        return;
    cout << "Name:" << ptr->name << " Wife:" << ptr->partnerName << endl;
}
void familyTree::printGenerationNumByName(string name)
{
    cout << findGenerationNum(findName(name, oldest), oldest, 1) << endl;
}

void familyTree::traverse(Person *ptr)
{
    while (1)
    {
        cout << "Name:" << ptr->name << " Wife:" << ptr->partnerName << endl;
        if (ptr->junior != nullptr)
            traverse(ptr->junior);
        if (ptr->compeer != nullptr)
            ptr = ptr->compeer;
        else
            break;
    }
}
void familyTree::traverseCompeer(Person *ptr)
{
    if (ptr != nullptr)
    {
        cout << "Name:" << ptr->name << " Wife:" << ptr->partnerName << endl;
        traverseCompeer(ptr->compeer);
    }
}

void familyTree::clear(Person *ptr)
{
    if (ptr != nullptr)
    {
        clear(ptr->compeer);
        clear(ptr->junior);
        delete ptr;
    }
}

void familyTree::copyTree(const Person *Source_Root, Person *&Target_Root)
{
    if (Source_Root->junior != nullptr)
    {
        Person *l = new Person(Source_Root->junior->name, Source_Root->junior->partnerName,
                               Source_Root->junior->born_date, Source_Root->junior->dead_date, Source_Root->junior->isMale);
        copyTree(Source_Root->junior, l); //copy the left;
        Target_Root->junior = l;
    }
    if (Source_Root->compeer != nullptr)
    {
        Person *r = new Person(Source_Root->compeer->name, Source_Root->compeer->partnerName,
                               Source_Root->compeer->born_date, Source_Root->compeer->dead_date, Source_Root->compeer->isMale);
        copyTree(Source_Root->compeer, r); //copy the right;
        Target_Root->compeer = r;
    }
}

Person *familyTree::findName(string name_, Person *ptr)
{
    if (ptr != nullptr) //preorder
    {
        if (ptr->name == name_ || ptr->partnerName == name_)
            return ptr;
        auto jun_ptr = findName(name_, ptr->junior);
        auto com_ptr = findName(name_, ptr->compeer);
        if (jun_ptr != nullptr)
            return jun_ptr;
        if (com_ptr != nullptr)
            return com_ptr;
        return nullptr;
    }
    return nullptr;
}
Person *familyTree::findParent(Person *son, Person *ptr)
{
    if (ptr->junior != nullptr) //preorder
    {
        if (ptr->junior == son)
            return ptr;
        auto jun_ptr = findParent(son, ptr->junior);
        auto com_ptr = findParent(son, ptr->compeer);
        if (jun_ptr != nullptr)
            return jun_ptr;
        if (com_ptr != nullptr)
            return com_ptr;
        return nullptr;
    }
    return nullptr;
}
Person *familyTree::findPreCompeer(Person *guy)
{
    auto tmp = findBigCompeer(guy, oldest, oldest);
    if (tmp == guy)
        return nullptr;
    while (tmp->compeer != nullptr)
    {
        if (tmp->compeer == guy)
            return tmp;
        tmp = tmp->compeer;
    }
    return nullptr;
}

Person *familyTree::findBigCompeer(Person *guy, Person *ptr, Person *big)
{
    if (ptr != nullptr) //preorder
    {
        if (ptr == guy)
            return big;
        auto jun_ptr = findBigCompeer(guy, ptr->junior, ptr->junior);
        auto com_ptr = findBigCompeer(guy, ptr->compeer, big);
        if (jun_ptr != nullptr)
            return jun_ptr;
        if (com_ptr != nullptr)
            return com_ptr;
        return nullptr;
    }
    return nullptr;
}
Person *familyTree::findJunior(Person *ptr)
{
    if (ptr->junior != nullptr) //preorder
    {
        return ptr->junior;
    }
    else
        return nullptr;
}
int familyTree::findGenerationNum(Person *guy, Person *ptr, int gen)
{
    if (ptr != nullptr) //preorder
    {
        if (ptr == guy)
            return gen;
        auto jun = findGenerationNum(guy, ptr->junior, gen + 1);
        auto com = findGenerationNum(guy, ptr->compeer, gen);
        if(jun==-1&&com==-1)
            return -1;
        return jun > com ? jun : com;
    }
    return -1;
}
bool familyTree::changePerson(string name_, string partnerName_,
                              Date born, Date dead, bool isMale_)
{
    if ((findName(name_, oldest)!=nullptr)&&(findName(name_, oldest)!=currentPerson))
        return false;
    if (currentPerson != nullptr)
    {
        currentPerson->name = name_;
        currentPerson->partnerName = partnerName_;
        currentPerson->born_date = born;
        currentPerson->dead_date = dead;
        currentPerson->isMale = isMale_;
        return true;
    }
    return false;
}

string familyTree::getCurrentName() const
{
    if (currentPerson == nullptr)
        return "";
    else
        return currentPerson->name;
}
bool familyTree::changeCurrentPerson(string name)
{
    auto ptr = findName(name, oldest);
    if (ptr == nullptr)
        return false;
    currentPerson = ptr;
    return true;
}

bool familyTree::moveCurrentPerson(int pos)
{
    switch (pos)
    {
    case 8:
    {
        auto ptr = findParent(findBigCompeer(findName(currentPerson->name, oldest), oldest, oldest), oldest);
        if (ptr != nullptr)
        {
            currentPerson = ptr;
            return true;
        }
        return false;
    }
    case 2:
    {
        if (currentPerson->junior != nullptr)
        {
            currentPerson = currentPerson->junior;
            return true;
        }
        return false;
    }
    case 4:
    {
        auto ptr = findPreCompeer(currentPerson);
        if (ptr != nullptr)
        {
            currentPerson = ptr;
            return true;
        }
        return false;
    }
    case 6:
    {
        if (currentPerson->compeer != nullptr)
        {
            currentPerson = currentPerson->compeer;
            return true;
        }
        return false;
    }
    }
}

void familyTree::test_z()
{
    auto ptr = currentPerson;
    while (1)
    {
        char cmd;
        //cout << "test>";
        cin >> cmd;
        switch (cmd)
        {
        case 'e':
            return;
        case 'j':
        {
            ptr = ptr->junior;
            break;
        }
        case 'c':
        {
            ptr = ptr->compeer;
            break;
        }
        case 'l':
        {
            if (ptr->compeer == nullptr)
                //cout << "compeer null" << endl;
                if (ptr->junior == nullptr)
                    //cout << "junior null" << endl;
                    break;
        }
        case 'r':
        {
            clear(oldest);
            break;
        }
        case 't':
        {
            traverse(oldest);
            break;
        }
        case 'f':
        {
            string n;
            cin >> n;
            auto tmp = findPreCompeer(findName(n, oldest));
            if (tmp != nullptr)
                //cout<<tmp->name<<endl;
                ;
            break;
        }
        case 'x':
        {
            ofstream fileXML("users.xml");
            fileXML << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
            fileXML << "<?xml-stylesheet type=\"text/css\" href=\"xml.css\"?>\n";
            fileXML << "<main>\n";
            fileXML << toXML(oldest);
            fileXML << "</main>\n";
            fileXML.close();
        }
        case 'g':
        {
            cout<<findGenerationNum(currentPerson,oldest,0)<<endl;
            break;
        }
        }
    }
}
void familyTree::asd_show()
{
    asd_print(oldest, 0);
    cout << endl
         << endl;
    return;
}
void familyTree::asd_print(Person *ptr, int height)
{
    if (ptr == NULL)
    {
        return;
    }
    asd_print(ptr->compeer, height + 1);
    for (int i = 0; i < height; i++)
        cout << "     ";
    cout << ptr->name;
    if ((ptr->junior != NULL) && (ptr->compeer != NULL))
        cout << "<";
    else if (ptr->junior != NULL)
        cout << "\\";
    else if (ptr->compeer != NULL)
        cout << "/";
    cout << endl;
    asd_print(ptr->junior, height + 1);
    return;
}
//a a a1 1 a b b1 1 a c c1 0 a d d1 0 t x e e
void familyTree::writeToXML()
{
    ofstream fileXML("users.xml");
    fileXML << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    fileXML << "<?xml-stylesheet type=\"text/css\" href=\"xml.css\"?>\n";
    fileXML << "<main>\n";
    fileXML << toXML(oldest);
    fileXML << "</main>\n";
    fileXML.close();
    return;
}
string familyTree::toXML(Person *ptr)
{
    string temp;
    temp = "<Generation>" + to_string(findGenerationNum(ptr, oldest, 1)) + "\n";
    while (1)
    {
        temp += "<Person>\n";
        temp += "<Name>" + ptr->name + "</Name>\n";
        temp += "<PartnerName>" + ptr->partnerName + "</PartnerName>\n";
        temp += "<BornDate>" + Date::dateToString(ptr->born_date) + "</BornDate>\n";
        temp += "<DeadDate>" + Date::dateToString(ptr->born_date) + "</DeadDate>\n";
        temp += "<Gender>";
        temp += ptr->isMale ? "Male</Gender>\n" : "Female</Gender>\n";
        if (ptr->junior != nullptr)
            temp += toXML(ptr->junior);
        if (ptr->compeer != nullptr)
            ptr = ptr->compeer;
        else
        {
            temp += "</Person>\n";
            break;
        }
        temp += "</Person>\n";
    }
    temp += "</Generation>\n";
    return temp;
}
bool familyTree::fromFile()
{
    ifstream xml("users.xml");
    string line;
    while (getline(xml, line))
    {
        /*
        while (line.find("\"") != string::npos)
        {
            auto pos = line.find("\"");
            line.erase(pos, 1);
        }
        //cout<<line<<endl;
        istringstream strstream(line);
        string temp;
        User a;
        getline(strstream, temp, ',');
        a.setName(temp);
        getline(strstream, temp, ',');
        a.setPassword(temp);
        getline(strstream, temp, ',');
        a.setEmail(temp);
        getline(strstream, temp, ',');
        a.setPhone(temp);
        for (auto i : m_userList)
        {
            if (i.getName() == a.getName())
            {
                exist = true;
            }
        }
        if (!exist)
            m_userList.push_back(a);

        //cout<<a.getName()<<a.getPassword()<<a.getEmail()<<a.getPhone()<<endl;
        */
    }
    xml.close();
}
