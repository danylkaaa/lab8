//
// Created by zulus on 12.05.17.
//

#include <string>
#include <CounriesStorage.h>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

CounriesStorage::CounriesStorage() {}

string Country::toString() {
    char *jsonString = json_dumps(jsn, JSON_INDENT(2));
    string s(jsonString);
    free(jsonString);
    return s;
}

Country::~Country() {
    json_decref(jsn);
}

CounriesStorage *CounriesStorage::load(const std::string &filename) {
    CounriesStorage *storage = new CounriesStorage();
    ifstream t(filename);
    if (t.is_open()) {
        std::string str;
        t.seekg(0, std::ios::end);
        str.reserve(t.tellg());
        t.seekg(0, std::ios::beg);

        str.assign((std::istreambuf_iterator<char>(t)),
                   std::istreambuf_iterator<char>());
        storage->add(str);
        t.close();
    }
    return storage;
}

void CounriesStorage::add(Country *country) {
    if (country->getId() == 0) {
        country->setId(id++);
    }
    this->items[country->getId()] = country;
}

void CounriesStorage::add(std::string &jstr) {
    json_error_t err;
    //get array
    auto jDoc = json_loads(jstr.c_str(), 0, &err);
    const char *key;
    json_t *value;
    //for each object in document, parse and add it to map
    json_object_foreach(jDoc, key, value) {
        add(Country::load(value));
    }
    json_decref(jDoc);
}

bool nameFilter(Country *c, void *key) {
    return c->getName().compare(*(string *) key) == 0;
}

bool officialNameFilter(Country *c, void *key) {
    return c->getOfficialName().compare(*(string *) key) == 0;
}

bool abbrFilter(Country *c, void *key) {
    return c->getAbbr().compare(*(string *) key) == 0;
}

bool idFilter(Country *c, void *key) {
    return c->getId() == stoi(*(string *) key);
}


string CounriesStorage::get(bool(*isValid)(Country *c, void *key), const string &key) {
    auto jarr = json_array();
    //get elements by filter
    //convert them to json string
    for (auto it = items.begin(); it != items.end(); it++) {
        if (isValid(it->second, (void *) &key)) {
            json_array_append_new(jarr, it->second->json());
        }
    }
    // create JSON document string
    char *jsonString = json_dumps(jarr, JSON_INDENT(2));
    string jstr(jsonString);
    free(jsonString);
    json_decref(jarr);
    return jstr;
}

string CounriesStorage::get(const string &key, const string &value) {
    if (key.compare("name") == 0) {
        return get(nameFilter, value);
    } else if (key.compare("officialName") == 0) {
        return get(officialNameFilter, value);
    } else if (key.compare("abbr") == 0) {
        return get(abbrFilter, value);
    } else if (key.compare("id") == 0) {
        return get(idFilter, value);
    } else return "";
}

CounriesStorage::~CounriesStorage() {
    for (auto it = items.begin(); it != items.end(); it++) {
        delete it->second;
    }
    items.clear();
}
