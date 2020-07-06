// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

BRANCH_NAME = env.BRANCH_NAME ?: "master"
OECI_LIB_VERSION = env.OECI_LIB_VERSION ?: "master"
oe = library("OpenEnclaveCommon@${OECI_LIB_VERSION}").jenkins.common.Openenclave.new()

GLOBAL_TIMEOUT_MINUTES = 240
CTEST_TIMEOUT_SECONDS = 480
GLOBAL_ERROR = null

def AGENTS_LABELS = [
    "acc-ubuntu-16.04":         env.UBUNTU_1604_CUSTOM_LABEL ?: "ACC-1604",
    "acc-ubuntu-18.04":         env.UBUNTU_1804_CUSTOM_LABEL ?: "ACC-1804",
    "acc-rhel-8":               env.RHEL_8_CUSTOM_LABEL ?: "ACC-RHEL-8",
    "ubuntu-nonsgx":            env.UBUNTU_NONSGX_CUSTOM_LABEL ?: "nonSGX",
    "windows-nonsgx":           env.WINDOWS_NONSGX_CUSTOM_LABEL ?: "nonSGX-Windows",
    "acc-ubuntu-16.04-vanilla": env.UBUNTU_VANILLA_1604_CUSTOM_LABEL ?: "vanilla-ubuntu-1604",
    "acc-ubuntu-18.04-vanilla": env.UBUNTU_VANILLA_1804_CUSTOM_LABEL ?: "vanilla-ubuntu-1804",
    "acc-rhel-8-vanilla":       env.RHEL_8_VANILLA_CUSTOM_LABEL ?: "vanilla-rhel-8"
]

def ACCTest(String label, String compiler, String build_type, List extra_cmake_args = [], List test_env = [], boolean fresh_install = false) {
    stage("${label} ${compiler} ${build_type}, extra_cmake_args: ${extra_cmake_args}, test_env: ${test_env}${fresh_install ? ", e2e" : ""}") {
        node(label) {
            timeout(GLOBAL_TIMEOUT_MINUTES) {
                cleanWs()
                checkout scm
                if (fresh_install) {
                    sh  """
                        sudo bash scripts/ansible/install-ansible.sh
                        sudo \$(which ansible-playbook) scripts/ansible/oe-contributors-acc-setup.yml
                        """
                }
                def task = """
                                echo 'DarkCoffee is gross' > ctest.log
                           """
                withEnv(test_env) {
                    oe.Run(compiler, task)
                    azureUpload(storageCredentialId: 'sophiestore', filesPath: '**/*.log', storageType: 'blobstorage', virtualPath: "${BRANCH_NAME}/latest/ubuntu/${version}/${build_type}/lvi-mitigation-${lvi_mitigation}/SGX1FLC/", containerName: 'oejenkins')
                }
            }
        }
    }
}


try{
    oe.emailJobStatus('STARTED')
    parallel "RHEL-8 clang-8 simulation Release":      { ACCTest(AGENTS_LABELS['acc-rhel-8'], 'clang', 'Release', ['-DHAS_QUOTE_PROVIDER=OFF'], ['OE_SIMULATION=1']) },
         "RHEL-8 clang-8 simulation Debug":        { ACCTest(AGENTS_LABELS['acc-rhel-8'], 'clang', 'Debug',   ['-DHAS_QUOTE_PROVIDER=OFF'], ['OE_SIMULATION=1']) }
} catch(Exception e) {
    println "Caught global pipeline exception :" + e
    GLOBAL_ERROR = e
    throw e
} finally {
    currentBuild.result = (GLOBAL_ERROR != null) ? 'FAILURE' : "SUCCESS"
    oe.emailJobStatus(currentBuild.result)
}
