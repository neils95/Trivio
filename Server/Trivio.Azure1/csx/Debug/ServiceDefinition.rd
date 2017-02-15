<?xml version="1.0" encoding="utf-8"?>
<serviceModel xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" name="Trivio.Azure1" generation="1" functional="0" release="0" Id="21157216-605a-4e48-9b95-948e3a26793c" dslVersion="1.2.0.0" xmlns="http://schemas.microsoft.com/dsltools/RDSM">
  <groups>
    <group name="Trivio.Azure1Group" generation="1" functional="0" release="0">
      <componentports>
        <inPort name="Trivio:Endpoint1" protocol="http">
          <inToChannel>
            <lBChannelMoniker name="/Trivio.Azure1/Trivio.Azure1Group/LB:Trivio:Endpoint1" />
          </inToChannel>
        </inPort>
      </componentports>
      <settings>
        <aCS name="Trivio:APPINSIGHTS_INSTRUMENTATIONKEY" defaultValue="">
          <maps>
            <mapMoniker name="/Trivio.Azure1/Trivio.Azure1Group/MapTrivio:APPINSIGHTS_INSTRUMENTATIONKEY" />
          </maps>
        </aCS>
        <aCS name="Trivio:Microsoft.WindowsAzure.Plugins.Diagnostics.ConnectionString" defaultValue="">
          <maps>
            <mapMoniker name="/Trivio.Azure1/Trivio.Azure1Group/MapTrivio:Microsoft.WindowsAzure.Plugins.Diagnostics.ConnectionString" />
          </maps>
        </aCS>
        <aCS name="TrivioInstances" defaultValue="[1,1,1]">
          <maps>
            <mapMoniker name="/Trivio.Azure1/Trivio.Azure1Group/MapTrivioInstances" />
          </maps>
        </aCS>
      </settings>
      <channels>
        <lBChannel name="LB:Trivio:Endpoint1">
          <toPorts>
            <inPortMoniker name="/Trivio.Azure1/Trivio.Azure1Group/Trivio/Endpoint1" />
          </toPorts>
        </lBChannel>
      </channels>
      <maps>
        <map name="MapTrivio:APPINSIGHTS_INSTRUMENTATIONKEY" kind="Identity">
          <setting>
            <aCSMoniker name="/Trivio.Azure1/Trivio.Azure1Group/Trivio/APPINSIGHTS_INSTRUMENTATIONKEY" />
          </setting>
        </map>
        <map name="MapTrivio:Microsoft.WindowsAzure.Plugins.Diagnostics.ConnectionString" kind="Identity">
          <setting>
            <aCSMoniker name="/Trivio.Azure1/Trivio.Azure1Group/Trivio/Microsoft.WindowsAzure.Plugins.Diagnostics.ConnectionString" />
          </setting>
        </map>
        <map name="MapTrivioInstances" kind="Identity">
          <setting>
            <sCSPolicyIDMoniker name="/Trivio.Azure1/Trivio.Azure1Group/TrivioInstances" />
          </setting>
        </map>
      </maps>
      <components>
        <groupHascomponents>
          <role name="Trivio" generation="1" functional="0" release="0" software="C:\Users\neils\Documents\BU\Spring 2017\EC464\Code\Trivio\Server\Trivio.Azure1\csx\Debug\roles\Trivio" entryPoint="base\x64\WaHostBootstrapper.exe" parameters="base\x64\WaIISHost.exe " memIndex="-1" hostingEnvironment="frontendadmin" hostingEnvironmentVersion="2">
            <componentports>
              <inPort name="Endpoint1" protocol="http" portRanges="80" />
            </componentports>
            <settings>
              <aCS name="APPINSIGHTS_INSTRUMENTATIONKEY" defaultValue="" />
              <aCS name="Microsoft.WindowsAzure.Plugins.Diagnostics.ConnectionString" defaultValue="" />
              <aCS name="__ModelData" defaultValue="&lt;m role=&quot;Trivio&quot; xmlns=&quot;urn:azure:m:v1&quot;&gt;&lt;r name=&quot;Trivio&quot;&gt;&lt;e name=&quot;Endpoint1&quot; /&gt;&lt;/r&gt;&lt;/m&gt;" />
            </settings>
            <resourcereferences>
              <resourceReference name="DiagnosticStore" defaultAmount="[4096,4096,4096]" defaultSticky="true" kind="Directory" />
              <resourceReference name="EventStore" defaultAmount="[1000,1000,1000]" defaultSticky="false" kind="LogStore" />
            </resourcereferences>
          </role>
          <sCSPolicy>
            <sCSPolicyIDMoniker name="/Trivio.Azure1/Trivio.Azure1Group/TrivioInstances" />
            <sCSPolicyUpdateDomainMoniker name="/Trivio.Azure1/Trivio.Azure1Group/TrivioUpgradeDomains" />
            <sCSPolicyFaultDomainMoniker name="/Trivio.Azure1/Trivio.Azure1Group/TrivioFaultDomains" />
          </sCSPolicy>
        </groupHascomponents>
      </components>
      <sCSPolicy>
        <sCSPolicyUpdateDomain name="TrivioUpgradeDomains" defaultPolicy="[5,5,5]" />
        <sCSPolicyFaultDomain name="TrivioFaultDomains" defaultPolicy="[2,2,2]" />
        <sCSPolicyID name="TrivioInstances" defaultPolicy="[1,1,1]" />
      </sCSPolicy>
    </group>
  </groups>
  <implements>
    <implementation Id="8fee715a-cbba-47a3-8429-8ea141d82e43" ref="Microsoft.RedDog.Contract\ServiceContract\Trivio.Azure1Contract@ServiceDefinition">
      <interfacereferences>
        <interfaceReference Id="837ea4fd-a775-4b66-b930-dcfc8bd3f9c6" ref="Microsoft.RedDog.Contract\Interface\Trivio:Endpoint1@ServiceDefinition">
          <inPort>
            <inPortMoniker name="/Trivio.Azure1/Trivio.Azure1Group/Trivio:Endpoint1" />
          </inPort>
        </interfaceReference>
      </interfacereferences>
    </implementation>
  </implements>
</serviceModel>