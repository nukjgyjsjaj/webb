#!/bin/bash
# Setup Local CI Server for Amnesia Browser
# This script sets up a local Jenkins/GitHub Actions-compatible CI server

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

install_docker() {
    if ! command -v docker &> /dev/null; then
        log_info "Installing Docker..."
        curl -fsSL https://get.docker.com -o get-docker.sh
        sudo bash get-docker.sh
        sudo usermod -aG docker $USER
    else
        log_info "Docker already installed"
    fi
}

install_jenkins() {
    log_info "Setting up Jenkins container..."
    
    docker pull jenkins/jenkins:lts
    
    docker run -d \
        --name jenkins \
        --restart=unless-stopped \
        -p 8080:8080 \
        -p 50000:50000 \
        -v jenkins_home:/var/jenkins_home \
        -v "$PROJECT_ROOT":/workspace/amnesia \
        -v /var/run/docker.sock:/var/run/docker.sock \
        jenkins/jenkins:lts
    
    log_info "Jenkins running at http://localhost:8080"
    log_info "Initial admin password:"
    docker exec jenkins cat /var/jenkins_home/secrets/initialAdminPassword
}

install_github_runner() {
    log_info "Setting up GitHub Actions runner..."
    
    RUNNER_DIR="$PROJECT_ROOT/github-runner"
    mkdir -p "$RUNNER_DIR"
    cd "$RUNNER_DIR"
    
    # Get runner token from GitHub (manual step)
    log_warn "Please create a GitHub Actions runner at:"
    log_warn "https://github.com/amnesia-browser/amnesia/settings/actions/runners"
    log_warn "Then enter the token below"
    
    read -p "Enter runner token: " RUNNER_TOKEN
    
    if [ -z "$RUNNER_TOKEN" ]; then
        log_error "Runner token is required"
        exit 1
    fi
    
    ./config.sh --url https://github.com/amnesia-browser/amnesia --token "$RUNNER_TOKEN"
    
    log_info "GitHub Actions runner configured"
}

setup_local_ci() {
    log_info "Setting up local CI infrastructure..."
    
    # Create CI user
    if ! id "ci-user" &>/dev/null; then
        sudo useradd -m -s /bin/bash ci-user
    fi
    
    # Setup directories
    sudo mkdir -p /opt/ci/{workspace,cache,logs}
    sudo chown -R ci-user:ci-user /opt/ci
    
    # Install dependencies
    log_info "Installing CI dependencies..."
    sudo apt-get update
    sudo apt-get install -y docker.io git python3 python3-pip
    
    # Install Docker Compose
    if ! command -v docker-compose &> /dev/null; then
        sudo curl -L "https://github.com/docker/compose/releases/latest/download/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
        sudo chmod +x /usr/local/bin/docker-compose
    fi
    
    log_info "Local CI infrastructure setup complete"
}

show_menu() {
    echo "Amnesia Browser CI Setup"
    echo "========================="
    echo "1. Install Docker"
    echo "2. Setup Jenkins"
    echo "3. Setup GitHub Actions Runner"
    echo "4. Full Local CI Setup"
    echo "5. Exit"
    echo
    read -p "Select option: " choice
    
    case $choice in
        1) install_docker ;;
        2) install_jenkins ;;
        3) install_github_runner ;;
        4) setup_local_ci ;;
        5) exit 0 ;;
        *) log_error "Invalid option" ;;
    esac
}

# Main
if [[ "${1:-}" == "--menu" ]]; then
    show_menu
else
    setup_local_ci
fi
