#include<iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <json.hpp>

using namespace std;
  #pragma region curl handling incoming data
    size_t callback(void* cont, size_t size, size_t mem, string* outbuf){
        size_t sum = size * mem;
        if(outbuf!=nullptr){
            outbuf->append(static_cast<char*>(cont),sum);
            return sum;
        }
            return 0; //returning anything  other than sum signals error to curl
    }
    #pragma endregion

int main(){
    #pragma region User Input
    string username,url;
    cout<<"Enter a github username(ensure proper upper and lower case chars): ";
    cin>>username;
    if(username.empty()){
        cout<<"Username cannot be empty"<<endl;
        return 1;
    }
    else{
        cout<<"User activity as follows: "<<endl;
        url="https://api.github.com/users/"+username+"/events";
    }
    #pragma endregion

  
    #pragma region curl setup
    curl_global_init(CURL_GLOBAL_DEFAULT);
    //curl handler
    CURL* curl = curl_easy_init();
    string resp;
    long http_code = 0;
    if(curl){
        string target = url;
        
        
        //target url
        //changed target.c_str() to url.c_str()
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        //setting callback and output destinations
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
        curl_easy_setopt(curl, CURLOPT_CAINFO, "curl-ca-bundle.crt");
        //setting http headers
        struct curl_slist* heads = nullptr;
        heads = curl_slist_append(heads, "User-Agent: Cpp-Github-CLI-App");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER,heads);
        const char* token = getenv("GITHUB_TOKEN");
        if(token!=nullptr){
            cout<<"[DEBUG]GITHUB_TOKEN loaded successfully!\n";
            string auth_header = "Authorization: token " + string(token);
            heads = curl_slist_append(heads, auth_header.c_str());
        }
        else {cout<<"[DEBUG]GITHUB_TOKEN not found.\n";}


        //http get request synchronous
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        CURLcode res = curl_easy_perform(curl);
        if(res == CURLE_OK){
            //checks http status codes like 404, 200...
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            cout<<"HTTP status code: "<<http_code<<endl;
            cout<<"Response size: "<<resp.size()<<" bytes"<<endl;}
        else cerr<<"curl_easy_perform() failed: "<<curl_easy_strerror(res)<<endl;
        curl_slist_free_all(heads);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    #pragma endregion

    #pragma region response validation
    if(http_code!=200){
        if(http_code==404){cout<<"Error: User '"<<username<<"' not found.\n";}
        else if (http_code == 403) {
        cout << "Error 403 Forbidden. GitHub Response:\n" << resp << "\n";
    }
        else{cout<<"Error: HTTP request failed with status code "<<http_code<<endl;}
        return 1;
    }
    #pragma endregion

    #pragma region json parsing
    using json = nlohmann::json;
    try{
        auto jsondata= nlohmann::json::parse(resp);
        if(jsondata.empty()){
            cout<<"No recent events for the user.\n";
            return 0;
        }
        cout<<"\nRecent Activity: \n";
        cout<<"-----------------\n";

        #pragma region event formattign n display
        for(const auto& event: jsondata){
            string type = event.value("type", "UnknownEvent");
            string repo_name = "unknown_repo";
            
            if(event.contains("repo")&& event["repo"].contains("name")){
                repo_name = event["repo"]["name"];
            }
            
            if(type =="PushEvent"){
               int commitc = 0;
               if(event.contains("payload") && event["payload"].contains("commits")) {
                    commitc = event["payload"]["commits"].size();
                }
                cout<<"- Pushed "<<commitc<<" commit(s) to "<<repo_name<<endl;
            }
            else if(type == "IssuesEvent"){
                string action = event["payload"].value("action", "updated");
                cout<<"- Opened a "<<action<<" issue in "<<repo_name<<endl;
            }
            else if(type=="WatchEvent"){
                cout<<"- Starred "<<repo_name<<endl;
            }
            else cout<<"- ..."<<endl;
        }
        #pragma endregion
    }
    //event processing
    catch(const nlohmann::json::parse_error& er){
        cerr<<"JSON parse error: "<<er.what()<<endl;
    }catch(const exception& e){
        cerr<<"Error processing event: "<<e.what()<<"\n";
    }
    #pragma endregion

return 0;
}